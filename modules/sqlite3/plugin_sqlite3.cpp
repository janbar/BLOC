/*
 *      Copyright (C) 2022 Jean-Luc Barriere
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "plugin_sqlite3.h"
#include <sqlite3.h>
#include <blocc/debug.h>
/*
 * Create the module SQLITE3Import
 */
PLUGINCREATOR(SQLITE3Plugin)

namespace bloc
{
namespace plugin
{
namespace SQLITE3
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "C", 0 }, // sqlite3
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // db filename
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Clone a database connection." },
  { 1,      1,  ctor_1_args,
          "Opening a new database connection." },
};

enum Method
{
  Open = 0, Close, IsOpen, Query1, Query2, Exec1, Exec2, ErrMsg,
  Prepare, Bind, Execute, Header, Fetch, Finalize,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG string_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG stmt_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // stmt
  { PLUGIN_IN,    { "R", 0 } }, // args
};

static PLUGIN_ARG bind_args[]  = {
  { PLUGIN_IN,    { "R", 0 } }, // args
};

static PLUGIN_ARG fetch_args[]   = {
  { PLUGIN_INOUT, { "R", 0 } }, // row
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Open,    "open",      { "B", 0 },     1, string_args,
          "Opening a new database connection." },
  { Close,   "close",     { "B", 0 },     0, nullptr,
          "Closing a database connection." },
  { IsOpen,   "isopen",   { "B", 0 },     0, nullptr,
          "Check a database connection is open." },
  { Query1,  "query",     { "R", 1 },     1, string_args,
          "One-step query execution." },
  { Query2,   "query",    { "R", 1 },     2, stmt_args,
          "One-step query execution with bound parameters." },
  { Exec1,    "exec",     { "B", 0 },     1, string_args,
          "One-step statement execution." },
  { Exec2,    "exec",     { "B", 0 },     2, stmt_args,
          "One-step statement execution with bound parameters." },
  { ErrMsg,   "errmsg",   { "L", 0 },     0, nullptr,
          "Return text that describes the error." },
  { Prepare,  "prepare",  { "B", 0 },     1, string_args,
          "Prepare a new statement object." },
  { Bind,     "bind",     { "B", 0 },     1, bind_args,
          "Bind parameters for the prepared statement." },
  { Execute,  "execute",  { "B", 0 },     0, nullptr,
          "Execute the prepared statement." },
  { Header,   "header",   { "R", 1 },     0, nullptr,
          "Return the header as table of tuple { name:string, type:string }." },
  { Fetch,    "fetch",    { "B", 0 },     1, fetch_args,
          "Fetch next row." },
  { Finalize, "finalize", { "B", 0 },     0, nullptr,
          "Close the prepared statement." },
};

/**
 * The state of handle
 */
struct Handle {
  std::string _path;
  struct sqlite3 * _db = nullptr;
  struct sqlite3_stmt * _stmt = nullptr;
  enum { STMT_NEW, STMT_ROW, STMT_DONE } _stmt_status = STMT_NEW;

  ~Handle()
  {
    if (_stmt)
      sqlite3_finalize(_stmt);
    if (_db)
      sqlite3_close(_db);
  }
  Handle() { }
  int open(const std::string& path);
  int close();
  int isOpen() { return (_db == nullptr ? 0 : 1); }
  int fetchall(bloc::Collection ** rs);
  int query(const std::string& str, bloc::Collection ** rs);
  int query(const std::string& str, const bloc::Tuple& args, bloc::Collection ** rs);
  int exec(const std::string& str);
  int exec(const std::string& str, const bloc::Tuple& args);
  const char * errmsg() { return sqlite3_errmsg(_db); }
  int prepare(const std::string& str);
  int bind(const bloc::Tuple& args);
  int execute();
  int header(bloc::Collection ** hd);
  int fetch(bloc::Tuple ** row);
  int finalize();
};

} /* namespace SQLITE3 */

void SQLITE3Plugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "sqlite3";
  interface->method_count = sizeof(SQLITE3::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = SQLITE3::methods;
  interface->ctors_count = sizeof(SQLITE3::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = SQLITE3::ctors;
}

void * SQLITE3Plugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  switch (ctor_id)
  {
  case 0: /* copy ctor */
  {
    SQLITE3::Handle * h = new SQLITE3::Handle();
    bloc::Complex c0 = args[0]->complex(ctx);
    std::string path = static_cast<SQLITE3::Handle*>(c0.instance())->_path;
    if (path.empty() || h->open(path) == 1)
      return h;
    delete h;
    break;
  }

  case 1:
  {
    std::string fn = args[0]->literal(ctx);
    SQLITE3::Handle * h = new SQLITE3::Handle();
    if (h->open(args[0]->literal(ctx)) == 1)
      return h;
    delete h;
    break;
  }

  default: /* default ctor */
    return new SQLITE3::Handle();
  }
  return nullptr;
}

void SQLITE3Plugin::destroyObject(void * object)
{
  SQLITE3::Handle * h = static_cast<SQLITE3::Handle*>(object);
  delete h;
}

bloc::Expression * SQLITE3Plugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  SQLITE3::Handle * h = static_cast<SQLITE3::Handle*>(object_this.instance());

  if (h->_db == nullptr && method_id != SQLITE3::Open)
    throw RuntimeError(EXC_RT_OTHER_S, "Database Connection not open.");

  switch (method_id)
  {
  case SQLITE3::Open:
    if (h->isOpen() == 1)
      h->close();
    return new bloc::BooleanExpression(h->open(args[0]->literal(ctx)));

  case SQLITE3::Close:
    if (h->isOpen() == 1)
      return new bloc::BooleanExpression(h->close());
    return new bloc::BooleanExpression(0);

  case SQLITE3::IsOpen:
    return new bloc::BooleanExpression(h->isOpen());

  case SQLITE3::Query1:
  {
    bloc::Collection * c;
    int r = h->query(args[0]->literal(ctx), &c);
    if (c)
    {
      bloc::CollectionExpression * e = new CollectionExpression(std::move(*c));
      delete c;
      return e;
    }
    return new bloc::CollectionExpression(bloc::Collection(bloc::Type().levelUp()));
  }

  case SQLITE3::Query2:
  {
    bloc::Collection * c;
    int r = h->query(args[0]->literal(ctx), args[1]->tuple(ctx), &c);
    if (c)
    {
      bloc::CollectionExpression * e = new CollectionExpression(std::move(*c));
      delete c;
      return e;
    }
    return new bloc::CollectionExpression(bloc::Collection(bloc::Type().levelUp()));
  }

  case SQLITE3::Exec1:
    return new bloc::BooleanExpression(h->exec(args[0]->literal(ctx)));

  case SQLITE3::Exec2:
    return new bloc::BooleanExpression(h->exec(args[0]->literal(ctx), args[1]->tuple(ctx)));

  case SQLITE3::ErrMsg:
    return new bloc::LiteralExpression(std::string(h->errmsg()));

  case SQLITE3::Prepare:
    return new bloc::BooleanExpression(h->prepare(args[0]->literal(ctx)));

  case SQLITE3::Bind:
    return new bloc::BooleanExpression(h->bind(args[0]->tuple(ctx)));

  case SQLITE3::Execute:
    return new bloc::BooleanExpression(h->execute());

  case SQLITE3::Header:
  {
    bloc::Collection * c;
    if (h->header(&c) != 1)
      throw RuntimeError(EXC_RT_OTHER_S, "No query in progress.");
    if (c)
    {
      bloc::CollectionExpression * e = new CollectionExpression(std::move(*c));
      delete c;
      return e;
    }
    return new bloc::CollectionExpression(bloc::Collection(bloc::Type().levelUp()));
  }

  case SQLITE3::Fetch:
  {
    bloc::Tuple * t;
    int r = h->fetch(&t);
    if (r == 1)
    {
      /* INOUT */
      bloc::VariableExpression * vout = dynamic_cast<bloc::VariableExpression*>(args[0]);
      vout->store(ctx, TupleExpression(std::move(*t)));
      delete t;
    }
    return new bloc::BooleanExpression(r);
  }

  case SQLITE3::Finalize:
    return new bloc::BooleanExpression(h->finalize());

  default:
    break;
  }
  return nullptr;
}

int SQLITE3::Handle::open(const std::string& path)
{
  int r = sqlite3_open(path.c_str(), &_db);
  if (r != SQLITE_OK)
    return 0;
  _path.assign(path);
  return 1;
}

int SQLITE3::Handle::close()
{
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  int r = sqlite3_close(_db);
  _db = nullptr;
  _path.clear();
  return (r == SQLITE_OK ? 1 : 0);
}

int SQLITE3::Handle::fetchall(bloc::Collection ** rs)
{
  /* fetch rows */
  int r = sqlite3_step(_stmt);
  if (r == SQLITE_ROW)
  {
    bloc::Tuple::Decl decl(sqlite3_column_count(_stmt), Type::COMPLEX);
    bloc::Collection::container_t c;
    do
    {
      std::vector<StaticExpression *> t;
      for (int i = 0; i < sqlite3_column_count(_stmt); ++i)
      {
        switch (sqlite3_column_type(_stmt, i))
        {
        case SQLITE_INTEGER:
          t.push_back(new bloc::IntegerExpression(sqlite3_column_int64(_stmt, i)));
          decl[i] = Type::INTEGER;
          break;
        case SQLITE_FLOAT:
          t.push_back(new bloc::NumericExpression(sqlite3_column_double(_stmt, i)));
          decl[i] = Type::NUMERIC;
          break;
        case SQLITE_TEXT:
          t.push_back(new bloc::LiteralExpression(std::string((const char*) sqlite3_column_text(_stmt, i))));
          decl[i] = Type::LITERAL;
          break;
        case SQLITE_BLOB:
        {
          int sz = sqlite3_column_bytes(_stmt, i);
          bloc::TabChar bytes((size_t)sz);
          const char * blob = (const char*) sqlite3_column_blob(_stmt, i);
          for (int p = 0; p < sz; ++p)
            bytes.push_back(blob[p]);
          t.push_back(new bloc::TabcharExpression(std::move(bytes)));
          decl[i] = Type::TABCHAR;
          break;
        }
        default:
          t.push_back(new bloc::ComplexExpression(bloc::Complex()));
          /* do not upgrade type */
          break;
        }
      }
      c.push_back(new bloc::TupleExpression(bloc::Tuple(std::move(t))));
      r = sqlite3_step(_stmt);
    } while (r == SQLITE_ROW);
    *rs = new bloc::Collection(decl, 1, std::move(c));
  }
  return (r == SQLITE_DONE ? 1 : 0);
}

int SQLITE3::Handle::query(const std::string& str, bloc::Collection ** rs)
{
  int r;
  const char * tail = nullptr;
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  *rs = nullptr;
  r = sqlite3_prepare_v2(_db, str.c_str(), str.size(), &_stmt, &tail);
  if (r != SQLITE_OK)
    return 0;
  r = fetchall(rs);
  sqlite3_finalize(_stmt);
  _stmt = nullptr;
  return r;
}

int SQLITE3::Handle::query(const std::string& str, const bloc::Tuple& args, bloc::Collection ** rs)
{
  int r;
  const char * tail = nullptr;
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  *rs = nullptr;
  r = sqlite3_prepare_v2(_db, str.c_str(), str.size(), &_stmt, &tail);
  if (r != SQLITE_OK)
    return 0;
  /* bind arguments */
  int i = 0;
  for (StaticExpression * e : args)
  {
    ++i;
    switch (e->refType().major())
    {
    case Type::BOOLEAN:
      sqlite3_bind_int(_stmt, i, (e->refBoolean() ? 1 : 0));
      break;
    case Type::INTEGER:
      sqlite3_bind_int64(_stmt, i, e->refInteger());
      break;
    case Type::NUMERIC:
      sqlite3_bind_double(_stmt, i, e->refNumeric());
      break;
    case Type::LITERAL:
      sqlite3_bind_text(_stmt, i, e->refLiteral().c_str(), e->refLiteral().size(), SQLITE_STATIC);
      break;
    case Type::TABCHAR:
      sqlite3_bind_blob(_stmt, i, e->refTabchar().data(), e->refTabchar().size(), SQLITE_STATIC);
      break;
    default:
      break;
    }
  }
  r = fetchall(rs);
  sqlite3_finalize(_stmt);
  _stmt = nullptr;
  return r;
}

int SQLITE3::Handle::exec(const std::string& str)
{
  int r;
  const char * tail = nullptr;
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  r = sqlite3_prepare_v2(_db, str.c_str(), str.size(), &_stmt, &tail);
  if (r != SQLITE_OK)
    return 0;
  r = sqlite3_step(_stmt);
  sqlite3_finalize(_stmt);
  _stmt = nullptr;
  return (r == SQLITE_DONE || r == SQLITE_ROW ? 1 : 0);
}

int SQLITE3::Handle::exec(const std::string& str, const bloc::Tuple& args)
{
  int r;
  const char * tail = nullptr;
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  r = sqlite3_prepare_v2(_db, str.c_str(), str.size(), &_stmt, &tail);
  if (r != SQLITE_OK)
    return 0;
  /* bind arguments */
  int i = 0;
  for (StaticExpression * e : args)
  {
    ++i;
    switch (e->refType().major())
    {
    case Type::BOOLEAN:
      sqlite3_bind_int(_stmt, i, (e->refBoolean() ? 1 : 0));
      break;
    case Type::INTEGER:
      sqlite3_bind_int64(_stmt, i, e->refInteger());
      break;
    case Type::NUMERIC:
      sqlite3_bind_double(_stmt, i, e->refNumeric());
      break;
    case Type::LITERAL:
      sqlite3_bind_text(_stmt, i, e->refLiteral().c_str(), e->refLiteral().size(), SQLITE_STATIC);
      break;
    case Type::TABCHAR:
      sqlite3_bind_blob(_stmt, i, e->refTabchar().data(), e->refTabchar().size(), SQLITE_STATIC);
      break;
    default:
      break;
    }
  }
  r = sqlite3_step(_stmt);
  sqlite3_finalize(_stmt);
  _stmt = nullptr;
  return (r == SQLITE_DONE || r == SQLITE_ROW ? 1 : 0);
}

int SQLITE3::Handle::prepare(const std::string& str)
{
  int r;
  const char * tail = nullptr;
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  _stmt_status = STMT_NEW;
  r = sqlite3_prepare_v2(_db, str.c_str(), str.size(), &_stmt, &tail);
  return (r == SQLITE_OK ? 1 : 0);
}

int SQLITE3::Handle::bind(const bloc::Tuple& args)
{
  if (_stmt == nullptr)
    return 0;
  _stmt_status = STMT_NEW;
  sqlite3_reset(_stmt);
  /* bind arguments */
  int i = 0;
  for (StaticExpression * e : args)
  {
    ++i;
    switch (e->refType().major())
    {
    case Type::BOOLEAN:
      sqlite3_bind_int(_stmt, i, (e->refBoolean() ? 1 : 0));
      break;
    case Type::INTEGER:
      sqlite3_bind_int64(_stmt, i, e->refInteger());
      break;
    case Type::NUMERIC:
      sqlite3_bind_double(_stmt, i, e->refNumeric());
      break;
    case Type::LITERAL:
      sqlite3_bind_text(_stmt, i, e->refLiteral().c_str(), e->refLiteral().size(), SQLITE_STATIC);
      break;
    case Type::TABCHAR:
      sqlite3_bind_blob(_stmt, i, e->refTabchar().data(), e->refTabchar().size(), SQLITE_STATIC);
      break;
    default:
      break;
    }
  }
  return 1;
}

int SQLITE3::Handle::execute()
{
  if (_stmt == nullptr)
    return 0;

  if (_stmt_status != STMT_NEW)
    sqlite3_reset(_stmt);

  switch (sqlite3_step(_stmt))
  {
  case SQLITE_DONE:
    _stmt_status = STMT_DONE;
    return 1;
  case SQLITE_ROW:
    _stmt_status = STMT_ROW;
    return 1;
  default:
    break;
  }
  return 0;
}

int SQLITE3::Handle::header(bloc::Collection ** hd)
{
  if (_stmt == nullptr || _stmt_status == STMT_NEW)
    return 0;
  *hd = nullptr;
  for (int i = 0; i < sqlite3_column_count(_stmt); ++i)
  {
    std::vector<StaticExpression *> t;
    t.push_back(new bloc::LiteralExpression(std::string(sqlite3_column_name(_stmt, i))));
    switch (sqlite3_column_type(_stmt, i))
    {
    case SQLITE_INTEGER:
      t.push_back(new bloc::LiteralExpression(bloc::Type::STR_INTEGER));
      break;
    case SQLITE_FLOAT:
      t.push_back(new bloc::LiteralExpression(bloc::Type::STR_NUMERIC));
      break;
    case SQLITE_TEXT:
      t.push_back(new bloc::LiteralExpression(bloc::Type::STR_LITERAL));
      break;
    case SQLITE_BLOB:
      t.push_back(new bloc::LiteralExpression(bloc::Type::STR_TABCHAR));
      break;
    default:
      /* complex null */
      t.push_back(new bloc::LiteralExpression(bloc::Type::STR_COMPLEX));
      break;
    }
    if (*hd)
      (*hd)->push_back(new bloc::TupleExpression(bloc::Tuple(std::move(t))));
    else
    {
      bloc::Tuple tuple(std::move(t));
      *hd = new bloc::Collection(tuple.tuple_decl(), 1);
      (*hd)->push_back(new bloc::TupleExpression(std::move(tuple)));
    }
  }
  return 1;
}

int SQLITE3::Handle::fetch(bloc::Tuple ** row)
{
  if (_stmt == nullptr)
    return 0;
  if (_stmt_status == STMT_ROW)
  {
    std::vector<StaticExpression *> t;
    for (int i = 0; i < sqlite3_column_count(_stmt); ++i)
    {
      switch (sqlite3_column_type(_stmt, i))
      {
      case SQLITE_INTEGER:
        t.push_back(new bloc::IntegerExpression(sqlite3_column_int64(_stmt, i)));
        break;
      case SQLITE_FLOAT:
        t.push_back(new bloc::NumericExpression(sqlite3_column_double(_stmt, i)));
        break;
      case SQLITE_TEXT:
        t.push_back(new bloc::LiteralExpression(std::string((const char*) sqlite3_column_text(_stmt, i))));
        break;
      case SQLITE_BLOB:
      {
        int sz = sqlite3_column_bytes(_stmt, i);
        bloc::TabChar bytes((size_t)sz);
        const char * blob = (const char*) sqlite3_column_blob(_stmt, i);
        for (int p = 0; p < sz; ++p)
          bytes.push_back(blob[p]);
        t.push_back(new bloc::TabcharExpression(std::move(bytes)));
        break;
      }
      default:
        t.push_back(new bloc::ComplexExpression(bloc::Complex()));
        break;
      }
    }
    *row = new bloc::Tuple(std::move(t));
    /* fetch next */
    if (sqlite3_step(_stmt) != SQLITE_ROW)
      _stmt_status = STMT_DONE;
    return 1;
  }
  return 0;
}

int SQLITE3::Handle::finalize()
{
  if (_stmt)
    sqlite3_finalize(_stmt);
  _stmt = nullptr;
  return 1;
}

} /* namespace import */
} /* namespace bloc */
