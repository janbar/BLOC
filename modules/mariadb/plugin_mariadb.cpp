/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#include "plugin_mariadb.h"
#include <mysql.h>
#include <blocc/exception_runtime.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/debug.h>
#include <string.h>
#include <cassert>

#define PLUGIN_TEXT_MAXLEN   0x0ffff
#define PLUGIN_TEXT_INILEN   0x00fff
#define PLUGIN_BLOB_MAXLEN   0x10000
#define PLUGIN_BLOB_INILEN   0x01000
/*
 * Create the module MariaDBImport
 */
PLUGINCREATOR(MariaDBPlugin)

namespace bloc
{
namespace plugin
{
namespace MariaDB
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "O", 0 }, // mariadb
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // host
  { "L", 0 }, // user
  { "L", 0 }, // password
  { "L", 0 }, // database
  { "I", 0 }, // port
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Clone a database connection." },
  { 1,      5,  ctor_1_args,
          "Opening a new database connection.\n"
  "1  : host\n2  : user\n3  : password\n4  : database\n5  : port" },
};

enum Method
{
  Open = 0, Close, IsOpen, AutoC, Commit, Rollback, Query1, Query2, Exec1,
  Exec2, ErrMsg, Prepare, Execute1, Execute2, Header, Fetch, Finalize,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG connect_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // host
  { PLUGIN_IN,    { "L", 0 } }, // user
  { PLUGIN_IN,    { "L", 0 } }, // password
  { PLUGIN_IN,    { "L", 0 } }, // dbname
  { PLUGIN_IN,    { "I", 0 } }, // port
};

static PLUGIN_ARG string_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG bool_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // boolean
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
  { Open,    "open",      { "B", 0 },     5, connect_args,
          "Opening a new database connection.\n"
  "1  : host\n2  : user\n3  : password\n4  : database\n5  : port" },
  { Close,   "close",     { "B", 0 },     0, nullptr,
          "Closing a database connection." },
  { IsOpen,   "isopen",   { "B", 0 },     0, nullptr,
          "Check a database connection is open." },
  { AutoC,  "autocommit", { "B", 0 },     1, bool_args,
          "Sets autocommit mode on or off." },
  { Commit,   "commit",     { "B", 0 },     0, nullptr,
          "Commits the current transaction, making its changes permanent." },
  { Rollback, "rollback",   { "B", 0 },     0, nullptr,
          "Rolls back the current transaction, canceling its changes." },
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
  { Execute1, "execute",  { "B", 0 },     1, bind_args,
          "Bind and execute the prepared statement." },
  { Execute2, "execute",  { "B", 0 },     0, nullptr,
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
  MYSQL * _db = nullptr;
  MYSQL_STMT * _stmt = nullptr;
  std::string _errmsg;

  ~Handle()
  {
    close();
  }
  Handle() { }
  int open(const std::string& host,
           const std::string& user,
           const std::string& passwd,
           const std::string& dbname,
           unsigned port);
  int close();
  int isOpen() const { return (_db && _db->db ? 1 : 0); }
  int autocommit(bool mode);
  int fetchall(MYSQL_STMT * stmt, bloc::Collection ** rs);
  int query(const std::string& str, bloc::Collection ** rs);
  int query(const std::string& str, bloc::Tuple& args, bloc::Collection ** rs);
  int exec(const std::string& str);
  int exec(const std::string& str, bloc::Tuple& args);
  const char * errmsg() const { return _errmsg.c_str(); }
  int prepare(const std::string& str);
  int execute(bloc::Tuple& args);
  int execute();
  int header(bloc::Collection ** hd);
  int fetch(bloc::Tuple ** row);
  int finalize();

  class Bindings {
    std::vector<MYSQL_BIND> _bindings;
  public:
    Bindings() { }
    explicit Bindings(unsigned size);
    explicit Bindings(const Bindings&) = delete;
    Bindings& operator=(const Bindings&) = delete;
    ~Bindings();
    void free_all();
    void reset(unsigned size);
    bool empty() const { return _bindings.empty(); }
    unsigned size() const { return _bindings.size(); }
    MYSQL_BIND * data() { return _bindings.data(); }
    std::vector<MYSQL_BIND>& operator()() { return _bindings; }
    static unsigned long alloc_buffer(MYSQL_BIND& b, unsigned long len);
    static void free_buffer(MYSQL_BIND& b);
  };

  Bindings _stmt_rs;
  bloc::TupleDecl::Decl _stmt_decl;

  static void bind_args(Bindings& bindings, bloc::Tuple& args);
  static void unbind_args(Bindings& bindings);
};

} /* namespace MySQL */

MariaDBPlugin::~MariaDBPlugin()
{
  /* clean up and free resources used by the library */
  mysql_library_end();
}

void MariaDBPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "mariadb";
  interface->method_count = sizeof(MariaDB::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = MariaDB::methods;
  interface->ctors_count = sizeof(MariaDB::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = MariaDB::ctors;
}

void * MariaDBPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  switch (ctor_id)
  {
  case 0: /* copy ctor */
  {
    MariaDB::Handle * h = nullptr;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull() || (h = static_cast<MariaDB::Handle*>(a0.complex()->instance())) == nullptr)
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    MariaDB::Handle * n = new MariaDB::Handle();
    if (h->isOpen())
    {
      if (n->open(h->_db->host, h->_db->user, h->_db->passwd, h->_db->db,
                  h->_db->port) == 1)
        return n;
      std::string msg(n->errmsg());
      delete n;
      throw RuntimeError(EXC_RT_USER_S, msg.c_str());
    }
    return n;
  }

  case 1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    MariaDB::Handle * h = new MariaDB::Handle();
    if (h->open(*a0.literal(), *a1.literal(), *a2.literal(), *a3.literal(),
                (unsigned)*a4.integer()) == 1)
      return h;
    delete h;
    break;
  }

  default: /* default ctor */
    return new MariaDB::Handle();
  }
  return nullptr;
}

void MariaDBPlugin::destroyObject(void * object)
{
  MariaDB::Handle * h = static_cast<MariaDB::Handle*>(object);
  delete h;
}

bloc::Value * MariaDBPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  MariaDB::Handle * h = static_cast<MariaDB::Handle*>(object_this.instance());

  switch (method_id)
  {
  case MariaDB::Open:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (h->isOpen() == 1)
      h->close();
    return new bloc::Value(bloc::Bool(h->open(*a0.literal(),
                                              *a1.literal(),
                                              *a2.literal(),
                                              *a3.literal(),
                                              (unsigned)*a4.integer())));
  }

  case MariaDB::Close:
    if (h->isOpen())
      return new bloc::Value(bloc::Bool(h->close()));
    return new bloc::Value(bloc::Bool(0));

  case MariaDB::IsOpen:
    return new bloc::Value(bloc::Bool(h->isOpen()));

  default:
    if (!h->isOpen())
      throw RuntimeError(EXC_RT_OTHER_S, "Database Connection not open.");
  }

  switch (method_id)
  {
  case MariaDB::AutoC:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->autocommit(*a0.boolean()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::Commit:
  {
    if (!h->exec("COMMIT"))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::Rollback:
  {
    if (!h->exec("ROLLBACK"))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::Query1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection * c = nullptr;
    if (!h->query(*a0.literal(), &c))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(c);
  }

  case MariaDB::Query2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection * c = nullptr;
    if (!h->query(*a0.literal(), *a1.tuple(), &c))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(c);
  }

  case MariaDB::Exec1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->exec(*a0.literal()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::Exec2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->exec(*a0.literal(), *a1.tuple()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::ErrMsg:
    return new bloc::Value(new bloc::Literal(std::string(h->errmsg())));

   case MariaDB::Prepare:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->prepare(*a0.literal()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::Execute1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->execute(*a0.tuple()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case MariaDB::Execute2:
    if (!h->execute())
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));

  case MariaDB::Header:
  {
    bloc::Collection * c = nullptr;
    int r = h->header(&c);
    return new bloc::Value(c);
  }

  case MariaDB::Fetch:
  {
    // INOUT //
    if (!args[0]->isVarName())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Tuple * t = nullptr;
    int r = h->fetch(&t);
    if (r == 1)
      ctx.storeVariable(*args[0]->symbol(), bloc::Value(t));
    return new bloc::Value(bloc::Bool(r));
  }

  case MariaDB::Finalize:
    return new bloc::Value(bloc::Bool(h->finalize()));

  default:
    break;
  }
  return nullptr;
}

MariaDB::Handle::Bindings::Bindings(unsigned size)
{
  reset(size);
}

MariaDB::Handle::Bindings::~Bindings()
{
  free_all();
}

void MariaDB::Handle::Bindings::reset(unsigned size)
{
  free_all();
  _bindings.resize(size);
  for (MYSQL_BIND& bind : _bindings)
  {
    /* using local storage */
    bind.length = &bind.length_value;
    bind.is_null = &bind.is_null_value;
    bind.error = &bind.error_value;
  }
}

void MariaDB::Handle::Bindings::free_all()
{
  for (MYSQL_BIND& bind : _bindings)
  {
    if (bind.buffer)
      delete [] (char*)bind.buffer;
    bind.buffer = nullptr;
    bind.buffer_length = 0;
  }
}

unsigned long MariaDB::Handle::Bindings::alloc_buffer(MYSQL_BIND& b, unsigned long len)
{
  assert(b.buffer_length < len);
  if (b.buffer)
    delete [] (char*)b.buffer;
  b.buffer = new char[len];
  b.buffer_length = (b.buffer ? len : 0);
  return b.buffer_length;
}

void MariaDB::Handle::Bindings::free_buffer(MYSQL_BIND& b)
{
  if (b.buffer)
    delete [] (char*)b.buffer;
  b.buffer = nullptr;
  b.buffer_length = 0;
}

void MariaDB::Handle::bind_args(Bindings& bindings, bloc::Tuple& args)
{
  static const char b1 = 1; /* to bind value true */
  static const char b0 = 0; /* to bind value false */
  int i = 0;
  for (MYSQL_BIND& bind : bindings())
  {
    if (args.size() > i)
    {
      bloc::Value& v = args[i];
      *bind.is_null = (v.isNull() ? 1 : 0);
      switch (v.type().major())
      {
      case Type::BOOLEAN:
        bind.buffer_type = MYSQL_TYPE_TINY;
        bind.buffer_length = 0;
        if (v.isNull())
          bind.buffer = nullptr;
        else
          bind.buffer = const_cast<char*>((*v.boolean() ? &b1 : &b0));
        break;
      case Type::INTEGER:
        bind.buffer_type = MYSQL_TYPE_LONGLONG;
        bind.buffer_length = 0;
        if (v.isNull())
          bind.buffer = nullptr;
        else
          bind.buffer = v.integer();
        break;
      case Type::NUMERIC:
        bind.buffer_type = MYSQL_TYPE_DOUBLE;
        bind.buffer_length = 0;
        if (v.isNull())
          bind.buffer = nullptr;
        else
          bind.buffer = v.numeric();
        break;
      case Type::LITERAL:
        bind.buffer_type = MYSQL_TYPE_STRING;
        if (v.isNull())
        {
          bind.buffer = nullptr;
          bind.buffer_length = 0;
          *bind.length = 0;
        }
        else
        {
          bind.buffer = const_cast<char*>(v.literal()->c_str());
          bind.buffer_length = v.literal()->size();
          *bind.length = v.literal()->size();
        }
        break;
      case Type::TABCHAR:
        bind.buffer_type = MYSQL_TYPE_BLOB;
        if (v.isNull())
        {
          bind.buffer = nullptr;
          bind.buffer_length = 0;
          *bind.length = 0;
        }
        else
        {
          bind.buffer = v.tabchar()->data();
          bind.buffer_length = v.tabchar()->size();
          *bind.length = v.tabchar()->size();
        }
        break;
      default:
        bind.buffer = nullptr;
        bind.buffer_length = 0;
        *bind.length = 0;
        *bind.is_null = 1;
        break;
      }
    }
    else
    {
      bind.buffer = nullptr;
      bind.buffer_length = 0;
      *bind.length = 0;
      *bind.is_null = 1;
    }
    ++i;
  }
}

void MariaDB::Handle::unbind_args(Bindings& bindings)
{
  /* assumes that the buffers have been bound with held value */
  for (MYSQL_BIND& bind : bindings())
  {
    bind.buffer = nullptr;
    bind.buffer_length = 0;
  }
}

int MariaDB::Handle::open(const std::string& host,
                        const std::string& user,
                        const std::string& password,
                        const std::string& dbname,
                        unsigned port)
{
  _db = mysql_init(_db);
  if (!mysql_real_connect(_db, host.c_str(), user.c_str(), password.c_str(),
                             dbname.c_str(), port, 0, 0))
  {
    return 0;
  }
  return 1;
}

int MariaDB::Handle::autocommit(bool mode)
{
  if (!mysql_autocommit(_db, (int)mode))
    return 1;
  _errmsg.assign(mysql_error(_db));
  return 0;
}

int MariaDB::Handle::fetchall(MYSQL_STMT * stmt, bloc::Collection ** rs)
{
  int r = 0;
  /* fetch result set meta information */
  MYSQL_RES * prepare_meta_result = mysql_stmt_result_metadata(stmt);
  if (!prepare_meta_result)
  {
    /* no result set */
    *rs = nullptr;
    return 1;
  }
  /* Get total columns in the query */
  unsigned column_count = mysql_num_fields(prepare_meta_result);
  /* initialize bindinds */
  Bindings bindings(column_count);
  bloc::TupleDecl::Decl decl(column_count, bloc::Type::NO_TYPE);

  /* fetch metadata of the result set */
  int i = 0;
  for (MYSQL_BIND& bind : bindings())
  {
    MYSQL_FIELD * field = mysql_fetch_field(prepare_meta_result);
    bind.buffer_type = field->type;
    switch (field->type)
    {
    case MYSQL_TYPE_TINY:
      Bindings::alloc_buffer(bind, 1);
      decl[i] = bloc::Type::INTEGER;
      break;
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_YEAR:
      Bindings::alloc_buffer(bind, 2);
      decl[i] = bloc::Type::INTEGER;
      break;
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_LONG:
      Bindings::alloc_buffer(bind, 4);
      decl[i] = bloc::Type::INTEGER;
      break;
    case MYSQL_TYPE_FLOAT:
      Bindings::alloc_buffer(bind, 4);
      decl[i] = bloc::Type::NUMERIC;
      break;
    case MYSQL_TYPE_LONGLONG:
      Bindings::alloc_buffer(bind, 8);
      decl[i] = bloc::Type::INTEGER;
      break;
    case MYSQL_TYPE_DOUBLE:
      Bindings::alloc_buffer(bind, 8);
      decl[i] = bloc::Type::NUMERIC;
      break;
    case MYSQL_TYPE_TIMESTAMP:
    case MYSQL_TYPE_TIME:
    case MYSQL_TYPE_DATE:
    case MYSQL_TYPE_DATETIME:
      Bindings::alloc_buffer(bind, sizeof(MYSQL_TIME));
      decl[i] = bloc::Type::LITERAL;
      break;
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
      Bindings::alloc_buffer(bind, field->length);
      decl[i] = bloc::Type::NUMERIC;
      break;
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_STRING:
      if (field->length > PLUGIN_TEXT_INILEN)
        Bindings::alloc_buffer(bind, PLUGIN_TEXT_INILEN);
      else
        Bindings::alloc_buffer(bind, field->length);
      decl[i] = bloc::Type::LITERAL;
      break;
    case MYSQL_TYPE_BIT:
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
      if (field->length > PLUGIN_BLOB_INILEN)
        Bindings::alloc_buffer(bind, PLUGIN_BLOB_INILEN);
      else
        Bindings::alloc_buffer(bind, field->length);
      decl[i] = bloc::Type::TABCHAR;
      break;
    default:
      /* use complex for unsupported datatype */
      decl[i] = bloc::Type::COMPLEX;
    }
    ++i;
  }

  /* free metedata */
  mysql_free_result(prepare_meta_result);

  /* bind the result buffers */
  if (mysql_stmt_bind_result(stmt, bindings.data()))
    _errmsg.assign(mysql_error(_db));
  else
  {
    r = 1;
    bloc::Collection::container_t c;
    while (r == 1)
    {
      int status = mysql_stmt_fetch(stmt);
      if (status == MYSQL_NO_DATA)
        break;
      if (status == 1)
      {
        _errmsg.assign(mysql_error(_db));
        r = 0;
        break;
      }

      std::vector<bloc::Value> t;
      int i = 0;
      for (MYSQL_BIND& bind : bindings())
      {
        switch (bind.buffer_type)
        {
        case MYSQL_TYPE_FLOAT:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_numeric));
          else
            t.push_back(bloc::Value(bloc::Numeric(*(float*)bind.buffer)));
          break;
        case MYSQL_TYPE_DOUBLE:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_numeric));
          else
            t.push_back(bloc::Value(bloc::Numeric(*(double*)bind.buffer)));
          break;
        case MYSQL_TYPE_TINY:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_integer));
          else
            t.push_back(bloc::Value(bloc::Integer(*(char*)bind.buffer)));
          break;
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_YEAR:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_integer));
          else
            t.push_back(bloc::Value(bloc::Integer(*(short int*)bind.buffer)));
          break;
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_integer));
          else
            t.push_back(bloc::Value(bloc::Integer(*(int*)bind.buffer)));
          break;
        case MYSQL_TYPE_LONGLONG:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_integer));
          else
            t.push_back(bloc::Value(bloc::Integer(*(long long int*)bind.buffer)));
          break;
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_numeric));
          else
            t.push_back(bloc::Value::parseNumeric(std::string((char*)bind.buffer, bind.buffer_length)));
          break;
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_literal));
          else
          {
            char buf[32];
            MYSQL_TIME * ts = (MYSQL_TIME*)bind.buffer;
            switch (ts->time_type)
            {
            case MYSQL_TIMESTAMP_DATE:
              /* iso date */
              snprintf(buf, 11, "%04d-%02d-%02d",
                       ts->year, ts->month, ts->day);
              break;
            case MYSQL_TIMESTAMP_DATETIME:
              /* iso8601 date and time */
              if (ts->second_part == 0)
                snprintf(buf, 20, "%04d-%02d-%02dT%02d:%02d:%02d",
                         ts->year, ts->month, ts->day,
                         ts->hour, ts->minute, ts->second);
              else
                snprintf(buf, 27, "%04d-%02d-%02dT%02d:%02d:%02d.%06u",
                         ts->year, ts->month, ts->day,
                         ts->hour, ts->minute, ts->second,
                         (unsigned)(ts->second_part));
              break;
            case MYSQL_TIMESTAMP_TIME:
              if (ts->second_part == 0)
                snprintf(buf, 9, "%02d:%02d:%02d",
                         ts->hour, ts->minute, ts->second);
              else
                snprintf(buf, 16, "%02d:%02d:%02d.%06u",
                         ts->hour, ts->minute, ts->second,
                         (unsigned)(ts->second_part));
              break;
            default:
              *buf = '\0';
            }
            t.push_back(bloc::Value(new bloc::Literal(buf)));
          }
          break;
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_literal));
          else
          {
            unsigned long rl = *bind.length;
            if (rl > 0)
            {
              if (rl <= bind.buffer_length)
                t.push_back(bloc::Value(new bloc::Literal((char*)bind.buffer, rl)));
              else if (bind.buffer_length >= PLUGIN_TEXT_MAXLEN)
                t.push_back(bloc::Value(new bloc::Literal((char*)bind.buffer, bind.buffer_length)));
              else
              {
                if (Bindings::alloc_buffer(bind, (rl > PLUGIN_TEXT_MAXLEN ? PLUGIN_TEXT_MAXLEN : rl)) &&
                      !mysql_stmt_fetch_column(stmt, &bind, i, 0))
                  t.push_back(bloc::Value(new bloc::Literal((char*)bind.buffer, bind.buffer_length)));
                else
                {
                  _errmsg.assign("Out of memory");
                  r = 0; /* break */
                }
              }
            }
            else
              t.push_back(bloc::Value(new bloc::Literal()));
          }
          break;
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
          if (*bind.is_null)
            t.push_back(bloc::Value(bloc::Value::type_tabchar));
          else
          {
            unsigned long rl = *bind.length;
            if (rl > 0)
            {
              if (rl <= bind.buffer_length)
                t.push_back(bloc::Value(new bloc::TabChar((char*)bind.buffer, (char*)bind.buffer + rl)));
              else if (bind.buffer_length >= PLUGIN_BLOB_MAXLEN)
                t.push_back(bloc::Value(new bloc::TabChar((char*)bind.buffer, (char*)bind.buffer + bind.buffer_length)));
              else
              {
                if (Bindings::alloc_buffer(bind, (rl > PLUGIN_BLOB_MAXLEN ? PLUGIN_BLOB_MAXLEN : rl)) &&
                      !mysql_stmt_fetch_column(stmt, &bind, i, 0))
                  t.push_back(bloc::Value(new bloc::TabChar((char*)bind.buffer, (char*)bind.buffer + bind.buffer_length)));
                else
                {
                  _errmsg.assign("Out of memory");
                  r = 0; /* break */
                }
              }
            }
            else
              t.push_back(bloc::Value(new bloc::TabChar()));
          }
          break;
        default:
          /* use complex for unsupported datatype */
          t.push_back(bloc::Value(bloc::Value::type_complex));
        }
        ++i;
      }
      c.push_back(bloc::Value(new bloc::Tuple(std::move(t))));
    }
    *rs = new bloc::Collection(decl, 1, std::move(c));
  }

  mysql_stmt_free_result(stmt);
  return r;
}

int MariaDB::Handle::close()
{
  if (!_db)
    return 0;
  finalize();
  mysql_close(_db);
  _db = nullptr;
  return 1;
}

int MariaDB::Handle::query(const std::string& str, bloc::Collection ** rs)
{
  int r = 0;
  MYSQL_STMT * stmt = mysql_stmt_init(_db);
  if (stmt)
  {
    if (mysql_stmt_prepare(stmt, str.c_str(), str.length()))
      _errmsg.assign(mysql_error(_db));
    else if (mysql_stmt_execute(stmt))
      _errmsg.assign(mysql_error(_db));
    else if (fetchall(stmt, rs))
      r = 1;
    mysql_stmt_close(stmt);
  }
  return r;
}

int MariaDB::Handle::query(const std::string& str, bloc::Tuple& args, bloc::Collection ** rs)
{
  int r = 0;
  MYSQL_STMT * stmt = mysql_stmt_init(_db);
  if (stmt)
  {
    if (mysql_stmt_prepare(stmt, str.c_str(), str.length()))
      _errmsg.assign(mysql_error(_db));
    else
    {
      /* bind arguments */
      Bindings bindings(mysql_stmt_param_count(stmt));
      if (bindings.size() != args.size())
        _errmsg.assign("Not all parameters bound");
      else
      {
        bind_args(bindings, args);
        /* bind the buffers */
        if (mysql_stmt_bind_param(stmt, bindings.data()) ||
          mysql_stmt_execute(stmt))
          _errmsg.assign(mysql_error(_db));
        else if (!fetchall(stmt, rs))
          _errmsg.assign(mysql_error(_db));
        else
          r = 1;
        unbind_args(bindings);
      }
    }
    mysql_stmt_close(stmt);
  }
  return r;
}

int MariaDB::Handle::exec(const std::string& str)
{
  int r = 0;
  MYSQL_STMT * stmt = mysql_stmt_init(_db);
  if (stmt)
  {
    if (!mysql_stmt_prepare(stmt, str.c_str(), str.length()) &&
            !mysql_stmt_execute(stmt))
      r = 1;
    else
      _errmsg.assign(mysql_error(_db));
    mysql_stmt_close(stmt);
  }
  return r;
}

int MariaDB::Handle::exec(const std::string& str, bloc::Tuple& args)
{
  int r = 0;
  MYSQL_STMT * stmt = mysql_stmt_init(_db);
  if (stmt)
  {
    if (!mysql_stmt_prepare(stmt, str.c_str(), str.length()))
    {
      /* bind arguments */
      Bindings bindings(mysql_stmt_param_count(stmt));
      if (bindings.size() != args.size())
        _errmsg.assign("Not all parameters bound");
      else
      {
        bind_args(bindings, args);
        /* bind the buffers */
        if (!mysql_stmt_bind_param(stmt, bindings.data()) &&
                !mysql_stmt_execute(stmt))
          r = 1;
        else
          _errmsg.assign(mysql_error(_db));
        unbind_args(bindings);
      }
    }
    mysql_stmt_close(stmt);
  }
  return r;
}

int MariaDB::Handle::prepare(const std::string& str)
{
  if (_stmt)
    finalize();
  _stmt = mysql_stmt_init(_db);
  if (_stmt)
  {
    if (!mysql_stmt_prepare(_stmt, str.c_str(), str.length()))
      return 1;
    _errmsg.assign(mysql_error(_db));
    mysql_stmt_close(_stmt);
    _stmt = nullptr;
  }
  return 0;
}

int MariaDB::Handle::execute(bloc::Tuple& args)
{
  int r = 0;
  if (_stmt == nullptr)
    return r;
  if (!_stmt_rs.empty())
  {
    _stmt_rs.reset(0);
    mysql_stmt_free_result(_stmt);
  }
  /* bind arguments */
  Bindings bindings(mysql_stmt_param_count(_stmt));
  if (bindings.size() != args.size())
    _errmsg.assign("Not all parameters bound");
  else
  {
    bind_args(bindings, args);
    /* bind the buffers */
    if (!mysql_stmt_bind_param(_stmt, bindings.data()) &&
            !mysql_stmt_execute(_stmt))
      r = 1;
    else
      _errmsg.assign(mysql_error(_db));
    unbind_args(bindings);
  }
  return r;
}

int MariaDB::Handle::execute()
{
  if (_stmt == nullptr)
    return 0;
  if (!_stmt_rs.empty())
  {
    _stmt_rs.reset(0);
    mysql_stmt_free_result(_stmt);
  }
  if (!mysql_stmt_execute(_stmt))
    return 1;
  _errmsg.assign(mysql_error(_db));
  return 0;
}

int MariaDB::Handle::header(bloc::Collection ** hd)
{
  if (_stmt == nullptr)
    throw RuntimeError(EXC_RT_OTHER_S, "No query in progress");
  /* fetch result set meta information */
  MYSQL_RES * prepare_meta_result = mysql_stmt_result_metadata(_stmt);
  if (!prepare_meta_result)
  {
    /* no result set */
    return 0;
  }
  /* Get total columns in the query */
  unsigned column_count = mysql_num_fields(prepare_meta_result);

  /* fetch metadata of the result set */
  for (int i = 0; i < column_count; ++i)
  {
    MYSQL_FIELD * field = mysql_fetch_field(prepare_meta_result);
    std::vector<bloc::Value> t;
    t.push_back(bloc::Value(new bloc::Literal(field->name, field->name_length)));
    switch (field->type)
    {
    case MYSQL_TYPE_TINY:
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_YEAR:
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_LONGLONG:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_INTEGER)));
      break;
    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DOUBLE:
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_NUMERIC)));
      break;
    case MYSQL_TYPE_TIMESTAMP:
    case MYSQL_TYPE_TIME:
    case MYSQL_TYPE_DATE:
    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_STRING:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_LITERAL)));
      break;
    case MYSQL_TYPE_BIT:
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_TABCHAR)));
      break;
    default:
      /* use complex for unsupported datatype */
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_COMPLEX)));
    }
    if (*hd)
      (*hd)->push_back(bloc::Value(new bloc::Tuple(std::move(t))));
    else
    {
      bloc::Tuple tuple(std::move(t));
      *hd = new bloc::Collection(tuple.tuple_decl(), 1);
      (*hd)->push_back(bloc::Value(new bloc::Tuple(std::move(tuple))));
    }
  }
  /* free metedata */
  mysql_free_result(prepare_meta_result);
  return 1;
}

int MariaDB::Handle::fetch(bloc::Tuple ** row)
{
  if (_stmt == nullptr)
    throw RuntimeError(EXC_RT_OTHER_S, "No query in progress");
  /* initialize bindinds */
  if (_stmt_rs.empty())
  {
    /* fetch result set meta information */
    MYSQL_RES * prepare_meta_result = mysql_stmt_result_metadata(_stmt);
    if (!prepare_meta_result)
    {
      /* no result set */
      return 0;
    }
    /* Get total columns in the query */
    unsigned column_count = mysql_num_fields(prepare_meta_result);
    _stmt_rs.reset(column_count);
    _stmt_decl.resize(column_count);

    /* fetch metadata of the result set */
    int i = 0;
    for (MYSQL_BIND& bind : _stmt_rs())
    {
      MYSQL_FIELD * field = mysql_fetch_field(prepare_meta_result);
      bind.buffer_type = field->type;
      switch (field->type)
      {
      case MYSQL_TYPE_TINY:
        Bindings::alloc_buffer(bind, 1);
        _stmt_decl[i] = bloc::Type::INTEGER;
        break;
      case MYSQL_TYPE_SHORT:
      case MYSQL_TYPE_YEAR:
        Bindings::alloc_buffer(bind, 2);
        _stmt_decl[i] = bloc::Type::INTEGER;
        break;
      case MYSQL_TYPE_INT24:
      case MYSQL_TYPE_LONG:
        Bindings::alloc_buffer(bind, 4);
        _stmt_decl[i] = bloc::Type::INTEGER;
        break;
      case MYSQL_TYPE_FLOAT:
        Bindings::alloc_buffer(bind, 4);
        _stmt_decl[i] = bloc::Type::NUMERIC;
        break;
      case MYSQL_TYPE_LONGLONG:
        Bindings::alloc_buffer(bind, 8);
        _stmt_decl[i] = bloc::Type::INTEGER;
        break;
      case MYSQL_TYPE_DOUBLE:
        Bindings::alloc_buffer(bind, 8);
        _stmt_decl[i] = bloc::Type::NUMERIC;
        break;
      case MYSQL_TYPE_TIMESTAMP:
      case MYSQL_TYPE_TIME:
      case MYSQL_TYPE_DATE:
      case MYSQL_TYPE_DATETIME:
        Bindings::alloc_buffer(bind, sizeof(MYSQL_TIME));
        _stmt_decl[i] = bloc::Type::LITERAL;
        break;
      case MYSQL_TYPE_DECIMAL:
      case MYSQL_TYPE_NEWDECIMAL:
        Bindings::alloc_buffer(bind, field->length);
        _stmt_decl[i] = bloc::Type::NUMERIC;
        break;
      case MYSQL_TYPE_VAR_STRING:
      case MYSQL_TYPE_STRING:
        if (field->length > PLUGIN_TEXT_INILEN)
          Bindings::alloc_buffer(bind, PLUGIN_TEXT_INILEN);
        else
          Bindings::alloc_buffer(bind, field->length);
        _stmt_decl[i] = bloc::Type::LITERAL;
        break;
      case MYSQL_TYPE_BIT:
      case MYSQL_TYPE_BLOB:
      case MYSQL_TYPE_TINY_BLOB:
      case MYSQL_TYPE_MEDIUM_BLOB:
      case MYSQL_TYPE_LONG_BLOB:
        if (field->length > PLUGIN_BLOB_INILEN)
          Bindings::alloc_buffer(bind, PLUGIN_BLOB_INILEN);
        else
          Bindings::alloc_buffer(bind, field->length);
        _stmt_decl[i] = bloc::Type::TABCHAR;
        break;
      default:
        /* use complex for unsupported datatype */
        _stmt_decl[i] = bloc::Type::COMPLEX;
      }
      ++i;
    }
    /* free metedata */
    mysql_free_result(prepare_meta_result);
    /* bind the result buffers */
    if (mysql_stmt_bind_result(_stmt, _stmt_rs.data()))
      throw RuntimeError(EXC_RT_OTHER_S, mysql_error(_db));
  }

  int status = mysql_stmt_fetch(_stmt);
  if (status == MYSQL_NO_DATA)
    return 0;
  if (status == 1)
    throw RuntimeError(EXC_RT_USER_S, mysql_error(_db));

  std::vector<bloc::Value> t;
  int i = 0;
  for (MYSQL_BIND& bind : _stmt_rs())
  {
    switch (bind.buffer_type)
    {
    case MYSQL_TYPE_FLOAT:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_numeric));
      else
        t.push_back(bloc::Value(bloc::Numeric(*(float*)bind.buffer)));
      break;
    case MYSQL_TYPE_DOUBLE:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_numeric));
      else
        t.push_back(bloc::Value(bloc::Numeric(*(double*)bind.buffer)));
      break;
    case MYSQL_TYPE_TINY:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_integer));
      else
        t.push_back(bloc::Value(bloc::Integer(*(char*)bind.buffer)));
      break;
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_YEAR:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_integer));
      else
        t.push_back(bloc::Value(bloc::Integer(*(short int*)bind.buffer)));
      break;
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_LONG:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_integer));
      else
        t.push_back(bloc::Value(bloc::Integer(*(int*)bind.buffer)));
      break;
    case MYSQL_TYPE_LONGLONG:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_integer));
      else
        t.push_back(bloc::Value(bloc::Integer(*(long long int*)bind.buffer)));
      break;
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_numeric));
      else
        t.push_back(bloc::Value::parseNumeric(std::string((char*)bind.buffer, bind.buffer_length)));
      break;
    case MYSQL_TYPE_TIMESTAMP:
    case MYSQL_TYPE_DATE:
    case MYSQL_TYPE_TIME:
    case MYSQL_TYPE_DATETIME:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_literal));
      else
      {
        char buf[32];
        MYSQL_TIME * ts = (MYSQL_TIME*)bind.buffer;
        switch (ts->time_type)
        {
        case MYSQL_TIMESTAMP_DATE:
          /* iso date */
          snprintf(buf, 11, "%04d-%02d-%02d",
                   ts->year, ts->month, ts->day);
          break;
        case MYSQL_TIMESTAMP_DATETIME:
          /* iso8601 date and time */
          if (ts->second_part == 0)
            snprintf(buf, 20, "%04d-%02d-%02dT%02d:%02d:%02d",
                     ts->year, ts->month, ts->day,
                     ts->hour, ts->minute, ts->second);
          else
            snprintf(buf, 27, "%04d-%02d-%02dT%02d:%02d:%02d.%06u",
                     ts->year, ts->month, ts->day,
                     ts->hour, ts->minute, ts->second,
                     (unsigned)(ts->second_part));
          break;
        case MYSQL_TIMESTAMP_TIME:
          if (ts->second_part == 0)
            snprintf(buf, 9, "%02d:%02d:%02d",
                     ts->hour, ts->minute, ts->second);
          else
            snprintf(buf, 16, "%02d:%02d:%02d.%06u",
                     ts->hour, ts->minute, ts->second,
                     (unsigned)(ts->second_part));
          break;
        default:
          *buf = '\0';
        }
        t.push_back(bloc::Value(new bloc::Literal(buf)));
      }
      break;
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_STRING:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_literal));
      else
      {
        unsigned long rl = *bind.length;
        if (rl > 0)
        {
          if (rl <= bind.buffer_length)
            t.push_back(bloc::Value(new bloc::Literal((char*)bind.buffer, rl)));
          else if (bind.buffer_length >= PLUGIN_TEXT_MAXLEN)
            t.push_back(bloc::Value(new bloc::Literal((char*)bind.buffer, bind.buffer_length)));
          else
          {
            if (Bindings::alloc_buffer(bind, (rl > PLUGIN_TEXT_MAXLEN ? PLUGIN_TEXT_MAXLEN : rl)) &&
                  !mysql_stmt_fetch_column(_stmt, &bind, i, 0))
              t.push_back(bloc::Value(new bloc::Literal((char*)bind.buffer, bind.buffer_length)));
            else
              throw RuntimeError(EXC_RT_USER_S, "Out of memory");
          }
        }
        else
          t.push_back(bloc::Value(new bloc::Literal()));
      }
      break;
    case MYSQL_TYPE_BIT:
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
      if (*bind.is_null)
        t.push_back(bloc::Value(bloc::Value::type_tabchar));
      else
      {
        unsigned long rl = *bind.length;
        if (rl > 0)
        {
          if (rl <= bind.buffer_length)
            t.push_back(bloc::Value(new bloc::TabChar((char*)bind.buffer, (char*)bind.buffer + rl)));
          else if (bind.buffer_length >= PLUGIN_BLOB_MAXLEN)
            t.push_back(bloc::Value(new bloc::TabChar((char*)bind.buffer, (char*)bind.buffer + bind.buffer_length)));
          else
          {
            if (Bindings::alloc_buffer(bind, (rl > PLUGIN_BLOB_MAXLEN ? PLUGIN_BLOB_MAXLEN : rl)) &&
                  !mysql_stmt_fetch_column(_stmt, &bind, i, 0))
              t.push_back(bloc::Value(new bloc::TabChar((char*)bind.buffer, (char*)bind.buffer + bind.buffer_length)));
            else
              throw RuntimeError(EXC_RT_USER_S, "Out of memory");
          }
        }
        else
          t.push_back(bloc::Value(new bloc::TabChar()));
      }
      break;
    default:
      /* use complex for unsupported datatype */
      t.push_back(bloc::Value(bloc::Value::type_complex));
    }
    ++i;
  }
  *row = new bloc::Tuple(std::move(t));
  return 1;
}

int MariaDB::Handle::finalize()
{
  if (!_stmt)
    return 0;
  if (!_stmt_rs.empty())
  {
    _stmt_rs.reset(0);
    mysql_stmt_free_result(_stmt);
  }
  mysql_stmt_close(_stmt);
  _stmt = nullptr;
  return 1;
}

} /* namespace import */
} /* namespace bloc */
