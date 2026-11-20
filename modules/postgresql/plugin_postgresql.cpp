/*
 *      Copyright (C) 2026 Jean-Luc Barriere
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

#include "plugin_postgresql.h"
#include <libpq-fe.h>
#include <blocc/exception_runtime.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/debug.h>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <forward_list>

#define PLUGIN_DATE_STYLE           "ISO,YMD"

/* Boolean & Binary */
#define OIDBOOL             16      /* boolean */
#define OIDBYTEA            17      /* bytea */

/* Character & Text */
#define OIDCHAR             18      /* "char" (internal single-byte) */
#define OIDNAME             19      /* name (internal 63-byte identifier) */
#define OIDTEXT             25      /* text */
#define OIDBPCHAR           1042    /* character(n) / char(n) */
#define OIDVARCHAR          1043    /* character varying(n) / varchar(n) */

/* Numeric & Monetary */
#define OIDINT8             20      /* bigint / int8 */
#define OIDINT2             21      /* smallint / int2 */
#define OIDINT4             23      /* integer / int4 */
#define OIDFLOAT4           700     /* real / float4 */
#define OIDFLOAT8           701     /* double precision / float8 */
#define OIDCASH             790     /* money */
#define OIDNUMERIC          1700    /* numeric / decimal */

/* Date & Time */
#define OIDDATE             1082    /* date */
#define OIDTIME             1083    /* time without time zone */
#define OIDTIMESTAMP        1114    /* timestamp without time zone */
#define OIDTIMESTAMPTZ      1184    /* timestamp with time zone */
#define OIDINTERVAL         1186    /* interval */
#define OIDTIMETZ           1266    /* time with time zone */

/* Network Address */
#define OIDCIDR             650     /* cidr */
#define OIDMACADDR8         774     /* macaddr8 */
#define OIDMACADDR          829     /* macaddr */
#define OIDINET             869     /* inet */

/* Bit String */
#define OIDBIT              1560    /* bit(n) */
#define OIDVARBIT           1562    /* bit varying(n) / varbit */

/* Document, Structured & UUID */
#define OIDJSON             114     /* json */
#define OIDXML              142     /* xml */
#define OIDUUID             2950    /* uuid */
#define OIDJSONB            3802    /* jsonb */
#define OIDJSONPATH         4072    /* jsonpath */

/*
 * Create the module PostgreSQLImport
 */
PLUGINCREATOR(PostgreSQLPlugin)

namespace bloc
{
namespace plugin
{
namespace PostgreSQL
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "O", 0 }, // postgresql
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
  Open = 0, Close, IsOpen, Commit, Rollback, Query1, Query2, Exec1,
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
  PGconn * _db = nullptr;
  std::string _connInfo;
  std::string _errmsg;

  ~Handle()
  {
    close();
  }
  Handle() {  }
  int open(const std::string& connInfo);
  int open(const std::string& host,
           const std::string& user,
           const std::string& passwd,
           const std::string& dbname,
           unsigned port);
  int close();
  int isOpen() const { return (_db ? 1 : 0); }
  int fetchall(PGresult * stmt, bloc::Collection ** rs);
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
  void cancel();

  typedef struct
  {
    Oid buffer_type;
    int buffer_length;
    std::string name;
  } PG_BIND;

  class Bindings {
    std::vector<PG_BIND> _bindings;
  public:
    Bindings() { }
    explicit Bindings(unsigned size);
    explicit Bindings(const Bindings&) = delete;
    Bindings& operator=(const Bindings&) = delete;
    ~Bindings() { }
    void clear() { _bindings.clear(); }
    void resize(unsigned size) { _bindings.resize(size); }
    bool empty() const { return _bindings.empty(); }
    unsigned size() const { return _bindings.size(); }
    std::vector<PG_BIND>& operator()() { return _bindings; }
  };

  bool _stmt_async = false;
  bool _stmt_prepared = false;
  Bindings _stmt_rs;
  bloc::TupleDecl::Decl _stmt_decl;

  typedef struct
  {
    std::forward_list<std::string> stack; /* temporary storage of values */
    std::vector<const char*> values;      /* array of pointers to values */
    std::vector<int> lengths;             /* array of value lengths */
    std::vector<int> formats;             /* array of value formats */
  } PG_PARAMS;

  static void bind_args(PG_PARAMS& params, bloc::Tuple& args);
};

} /* namespace MySQL */

PostgreSQLPlugin::~PostgreSQLPlugin()
{
  /* clean up and free resources used by the library */
}

void PostgreSQLPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "postgresql";
  interface->method_count = sizeof(PostgreSQL::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = PostgreSQL::methods;
  interface->ctors_count = sizeof(PostgreSQL::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = PostgreSQL::ctors;

  /* setup nls formats */
#if defined(LIBBLOC_MSWIN)
  std::string buf;
  buf.assign("PGDATESTYLE").append("=").append(PLUGIN_DATE_STYLE);
  _putenv(buf.c_str());
#else
  ::setenv("PGDATESTYLE", PLUGIN_DATE_STYLE, 1);
#endif
}

void * PostgreSQLPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  switch (ctor_id)
  {
  case 0: /* copy ctor */
  {
    PostgreSQL::Handle * h = nullptr;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull() || (h = static_cast<PostgreSQL::Handle*>(a0.complex()->instance())) == nullptr)
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    PostgreSQL::Handle * n = new PostgreSQL::Handle();
    if (h->isOpen())
    {
      if (n->open(h->_connInfo) == 1)
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
    PostgreSQL::Handle * h = new PostgreSQL::Handle();
    if (h->open(*a0.literal(), *a1.literal(), *a2.literal(), *a3.literal(),
                (unsigned)*a4.integer()) == 1)
      return h;
    delete h;
    break;
  }

  default: /* default ctor */
    return new PostgreSQL::Handle();
  }
  return nullptr;
}

void PostgreSQLPlugin::destroyObject(void * object)
{
  PostgreSQL::Handle * h = static_cast<PostgreSQL::Handle*>(object);
  delete h;
}

bloc::Value * PostgreSQLPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  PostgreSQL::Handle * h = static_cast<PostgreSQL::Handle*>(object_this.instance());

  switch (method_id)
  {
  case PostgreSQL::Open:
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

  case PostgreSQL::Close:
    if (h->isOpen())
      return new bloc::Value(bloc::Bool(h->close()));
    return new bloc::Value(bloc::Bool(0));

  case PostgreSQL::IsOpen:
    return new bloc::Value(bloc::Bool(h->isOpen()));

  default:
    if (!h->isOpen())
      throw RuntimeError(EXC_RT_OTHER_S, "Database Connection not open.");
  }

  switch (method_id)
  {
  case PostgreSQL::Commit:
  {
    if (!h->exec("COMMIT"))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case PostgreSQL::Rollback:
  {
    if (!h->exec("ROLLBACK"))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case PostgreSQL::Query1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection * c = nullptr;
    if (!h->query(*a0.literal(), &c))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(c);
  }

  case PostgreSQL::Query2:
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

  case PostgreSQL::Exec1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->exec(*a0.literal()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case PostgreSQL::Exec2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->exec(*a0.literal(), *a1.tuple()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case PostgreSQL::ErrMsg:
    return new bloc::Value(new bloc::Literal(std::string(h->errmsg())));

   case PostgreSQL::Prepare:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->prepare(*a0.literal()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case PostgreSQL::Execute1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->execute(*a0.tuple()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case PostgreSQL::Execute2:
    if (!h->execute())
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));

  case PostgreSQL::Header:
  {
    bloc::Collection * c = nullptr;
    (void) h->header(&c);
    return new bloc::Value(c);
  }

  case PostgreSQL::Fetch:
  {
    // INOUT //
    if (!args[0]->isVarName())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Tuple * t = nullptr;
    int r = h->fetch(&t);
    if (r == 1)
      ctx.storeVariable(args[0]->symbolId(), bloc::Value(t));
    return new bloc::Value(bloc::Bool(r));
  }

  case PostgreSQL::Finalize:
    return new bloc::Value(bloc::Bool(h->finalize()));

  default:
    break;
  }
  return nullptr;
}

PostgreSQL::Handle::Bindings::Bindings(unsigned size)
{
  resize(size);
}

void PostgreSQL::Handle::bind_args(PG_PARAMS& params, bloc::Tuple& args)
{
  params.values.clear();
  params.values.reserve(args.size());
  params.lengths.clear();
  params.lengths.reserve(args.size());
  params.formats.clear();
  params.formats.reserve(args.size());
  params.stack.clear();

  for (bloc::Value& v : args)
  {
    if (v.isNull())
    {
      params.values.push_back((char*)nullptr);
      params.lengths.push_back(0);
      params.formats.push_back(0);
    }
    else
    {
      switch (v.type().major())
      {
      case Type::BOOLEAN:
        /* use temporary storage */
        params.stack.push_front(std::string((*v.boolean() ? "t" : "f")));
        params.values.push_back(params.stack.front().c_str());
        params.lengths.push_back(params.stack.front().size());
        params.formats.push_back(0);
        break;
      case Type::INTEGER:
        /* use temporary storage */
        params.stack.push_front(bloc::Value::readableInteger(*v.integer()));
        params.values.push_back(params.stack.front().c_str());
        params.lengths.push_back(params.stack.front().size());
        params.formats.push_back(0);
        break;
      case Type::NUMERIC:
        params.stack.push_front(bloc::Value::readableNumeric(*v.numeric()));
        params.values.push_back(params.stack.front().c_str());
        params.lengths.push_back(params.stack.front().size());
        params.formats.push_back(0);
        break;
      case Type::IMAGINARY:
        /* use temporary storage */
        params.stack.push_front(bloc::Value::readableNumeric(v.imaginary()->a));
        params.values.push_back(params.stack.front().c_str());
        params.lengths.push_back(params.stack.front().size());
        params.formats.push_back(0);
        break;
      case Type::LITERAL:
        /* bind literal payload */
        params.values.push_back(v.literal()->c_str());
        params.lengths.push_back(v.literal()->size());
        params.formats.push_back(0);
        break;
      case Type::TABCHAR:
        /* bind binary payload */
        params.values.push_back(v.tabchar()->data());
        params.lengths.push_back(v.tabchar()->size());
        params.formats.push_back(1); /* binary format */
        break;
      default:
        /* cannot be bound, therefore bind to null */
        params.values.push_back((char*)nullptr);
        params.lengths.push_back(0);
        params.formats.push_back(0);
        break;
      }
    }
  }
}

int PostgreSQL::Handle::open(const std::string& connInfo)
{
  _connInfo.clear();
  _db = PQconnectdb(connInfo.c_str());
  if (!_db)
  {
    _errmsg.assign("Connection to database failed");
    return 0;
  }
  else if (PQstatus(_db) != CONNECTION_OK)
  {
    _errmsg.assign(PQerrorMessage(_db));
    PQfinish(_db);
    _db = nullptr;
    return 0;
  }
  _connInfo.assign(connInfo);
  return 1;
}

int PostgreSQL::Handle::open(const std::string& host,
                        const std::string& user,
                        const std::string& password,
                        const std::string& dbname,
                        unsigned port)
{
  std::string connInfo;
  if (!host.empty())
    connInfo.append(" host=").append(host);
  if (!user.empty())
    connInfo.append(" user=").append(user);
  if (!password.empty())
    connInfo.append(" password=").append(password);
  if (!dbname.empty())
    connInfo.append(" dbname=").append(dbname);
  if (port)
    connInfo.append(" port=").append(std::to_string(port));
  if (!connInfo.empty())
    return open(connInfo.substr(1));
  return 0;
}

int PostgreSQL::Handle::fetchall(PGresult * res, bloc::Collection ** rs)
{
  int r = 0;
  /* Get total columns in the query */
  int column_count = PQnfields(res);
  /* initialize bindinds */
  Bindings bindings(column_count);
  bloc::TupleDecl::Decl decl(column_count, bloc::Type::NO_TYPE);

  /* fetch metadata of the result set */
  int i = 0;
  for (PG_BIND& bind : bindings())
  {
    bind.buffer_type = PQftype(res, i);
    bind.buffer_length = PQfsize(res, i);
    switch (bind.buffer_type)
    {
    case OIDBOOL:
      decl[i] = bloc::Type::BOOLEAN;
      break;
    case OIDINT2:
    case OIDINT4:
    case OIDINT8:
      decl[i] = bloc::Type::INTEGER;
      break;
    case OIDFLOAT4:
    case OIDFLOAT8:
    case OIDCASH:
    case OIDNUMERIC:
      decl[i] = bloc::Type::NUMERIC;
      break;
    case OIDCHAR:
    case OIDNAME:
    case OIDTEXT:
    case OIDBPCHAR:
    case OIDVARCHAR:
      decl[i] = bloc::Type::LITERAL;
      break;
    case OIDDATE:
    case OIDTIME:
    case OIDTIMESTAMP:
    case OIDTIMESTAMPTZ:
    case OIDTIMETZ:
      decl[i] = bloc::Type::LITERAL;
      break;
    case OIDCIDR:
    case OIDMACADDR8:
    case OIDMACADDR:
    case OIDINET:
      decl[i] = bloc::Type::LITERAL;
      break;
    case OIDBYTEA:
    case OIDBIT:
    case OIDVARBIT:
      decl[i] = bloc::Type::TABCHAR;
      break;
    case OIDJSON:
    case OIDXML:
    case OIDUUID:
    case OIDJSONB:
    case OIDJSONPATH:
      decl[i] = bloc::Type::LITERAL;
      break;
    default:
      /* use complex for unsupported datatype */
      decl[i] = bloc::Type::COMPLEX;
    }
    ++i;
  }

  r = 1;
  int tuple_count = PQntuples(res);
  bloc::Collection::container_t c;
  for (int n = 0; n < tuple_count; ++n)
  {
    std::vector<bloc::Value> t;
    int i = 0;
    for (PG_BIND& bind : bindings())
    {
      int vnull = PQgetisnull(res, n, i);
      int vsize = PQgetlength(res, n, i);
      char * vdata = PQgetvalue(res, n, i);

      switch (bind.buffer_type)
      {
      case OIDBOOL:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_boolean));
        else
          t.push_back(bloc::Value(bloc::Bool(vdata[0] == 't' ? true : false)));
        break;
      case OIDINT2:
      case OIDINT4:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_integer));
        else
          t.push_back(bloc::Value(bloc::Integer(std::stoi(vdata))));
        break;
      case OIDINT8:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_integer));
        else
          t.push_back(bloc::Value(bloc::Integer(std::stoll(vdata))));
        break;
      case OIDFLOAT4:
      case OIDFLOAT8:
      case OIDCASH:
      case OIDNUMERIC:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_numeric));
        else
          t.push_back(bloc::Value(bloc::Numeric(std::stod(vdata))));
        break;
      case OIDCHAR:
      case OIDNAME:
      case OIDTEXT:
      case OIDBPCHAR:
      case OIDVARCHAR:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_literal));
        else
          t.push_back(bloc::Value(new bloc::Literal(vdata)));
        break;
      case OIDBYTEA:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_tabchar));
        else
        {
          size_t blen;
          unsigned char * braw = PQunescapeBytea((unsigned char*)vdata, &blen);
          if (!braw)
            t.push_back(bloc::Value(bloc::Value::type_tabchar));
          else
          {
            t.push_back(bloc::Value(new bloc::TabChar(braw, braw + blen)));
            PQfreemem(braw);
          }
        }
        break;
      case OIDDATE:
      case OIDTIME:
      case OIDTIMESTAMP:
      case OIDTIMESTAMPTZ:
      case OIDTIMETZ:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_literal));
        else
          t.push_back(bloc::Value(new bloc::Literal(vdata)));
        break;
      case OIDBIT:
      case OIDVARBIT:
      case OIDCIDR:
      case OIDMACADDR8:
      case OIDMACADDR:
      case OIDINET:
      case OIDJSON:
      case OIDXML:
      case OIDUUID:
      case OIDJSONB:
      case OIDJSONPATH:
        if (vnull)
          t.push_back(bloc::Value(bloc::Value::type_literal));
        else
          t.push_back(bloc::Value(new bloc::Literal(vdata)));
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

  return r;
}

int PostgreSQL::Handle::close()
{
  if (!_db)
    return 0;
  PQfinish(_db);
  _db = nullptr;
  return 1;
}

int PostgreSQL::Handle::query(const std::string& str, bloc::Collection ** rs)
{
  if (_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "Asynchronous query is in progress");

  int r = 0;
  PGresult * res = PQexec(_db, str.c_str());
  if (res)
  {
    ExecStatusType s = PQresultStatus(res);
    if (s == PGRES_COMMAND_OK)
      r = 1;
    else if (s == PGRES_TUPLES_OK)
      r = fetchall(res, rs);
    else
      _errmsg.assign(PQresultErrorMessage(res));
    PQclear(res);
  }
  else
    _errmsg.assign(PQerrorMessage(_db));
  return r;
}

int PostgreSQL::Handle::query(const std::string& str, bloc::Tuple& args, bloc::Collection ** rs)
{
  if (_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "Asynchronous query is in progress");

  int r = 0;
  PG_PARAMS params;
  /* bind arguments */
  bind_args(params, args);

  PGresult * res = PQexecParams(_db, str.c_str(), args.size(), nullptr,
                                params.values.data(),
                                params.lengths.data(),
                                params.formats.data(),
                                0);
  if (res)
  {
    ExecStatusType s = PQresultStatus(res);
    if (s == PGRES_COMMAND_OK)
      r = 1;
    else if (s == PGRES_TUPLES_OK)
      r = fetchall(res, rs);
    else
      _errmsg.assign(PQresultErrorMessage(res));
    PQclear(res);
  }
  else
    _errmsg.assign(PQerrorMessage(_db));
  return r;
}

int PostgreSQL::Handle::exec(const std::string& str)
{
  if (_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "Asynchronous query is in progress");

  int r = 0;
  PGresult * res = PQexec(_db, str.c_str());
  if (res)
  {
    ExecStatusType s = PQresultStatus(res);
    if (s == PGRES_COMMAND_OK || s == PGRES_TUPLES_OK)
      r = 1;
    else
      _errmsg.assign(PQresultErrorMessage(res));
    PQclear(res);
  }
  else
    _errmsg.assign(PQerrorMessage(_db));
  return r;
}

int PostgreSQL::Handle::exec(const std::string& str, bloc::Tuple& args)
{
  if (_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "Asynchronous query is in progress");

  int r = 0;
  PG_PARAMS params;
  /* bind arguments */
  bind_args(params, args);

  PGresult * res = PQexecParams(_db, str.c_str(), args.size(), nullptr,
                                params.values.data(),
                                params.lengths.data(),
                                params.formats.data(),
                                0);
  if (res)
  {
    ExecStatusType s = PQresultStatus(res);
    if (s == PGRES_COMMAND_OK || s == PGRES_TUPLES_OK)
      r = 1;
    else
      _errmsg.assign(PQresultErrorMessage(res));
    PQclear(res);
  }
  else
    _errmsg.assign(PQerrorMessage(_db));
  return r;
}

int PostgreSQL::Handle::prepare(const std::string& str)
{
  if (_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "Asynchronous query is in progress");

  _stmt_prepared = false;

  int r = 0;
  PGresult * res = PQprepare(_db, "", str.c_str(), 0, nullptr);
  if (res)
  {
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
      _errmsg.assign(PQresultErrorMessage(res));
    else
    {
      _stmt_prepared = true;
      r = 1;
    }
  }
  else
    _errmsg.assign(PQerrorMessage(_db));
  PQclear(res);
  return r;
}

int PostgreSQL::Handle::execute(bloc::Tuple& args)
{
  if (!_stmt_prepared)
    throw RuntimeError(EXC_RT_OTHER_S, "No prepared statement");

  if (_stmt_async)
    cancel();

  PG_PARAMS params;
  /* bind arguments */
  bind_args(params, args);

  if (PQsendQueryPrepared(_db, "", args.size(),
                          params.values.data(),
                          params.lengths.data(),
                          params.formats.data(),
                          0))
  {
    PQsetSingleRowMode(_db);
    _stmt_async = true;
    return 1;
  }

  _errmsg.assign(PQerrorMessage(_db));
  _stmt_async = false;
  cancel();
  return 0;
}

int PostgreSQL::Handle::execute()
{
  if (!_stmt_prepared)
    throw RuntimeError(EXC_RT_OTHER_S, "No prepared statement");

  if (_stmt_async)
    cancel();

  if (PQsendQueryPrepared(_db, "", 0, nullptr, nullptr, nullptr, 0))
  {
    PQsetSingleRowMode(_db);
    _stmt_async = true;
    return 1;
  }

  _errmsg.assign(PQerrorMessage(_db));
  _stmt_async = false;
  cancel();
  return 0;
}

int PostgreSQL::Handle::header(bloc::Collection ** hd)
{
  if (!_stmt_prepared)
    throw RuntimeError(EXC_RT_OTHER_S, "No prepared statement");

  if (_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "Asynchronous query is in progress");

  PGresult * desc = PQdescribePrepared(_db, "");
  if (PQresultStatus(desc) != PGRES_COMMAND_OK)
  {
    _errmsg.assign(PQresultErrorMessage(desc));
    PQclear(desc);
    return 0;
  }

  /* Get total columns in the query */
  int column_count = PQnfields(desc);
  /* fetch metadata of the result set */
  for (unsigned i = 0; i < column_count; ++i)
  {
    std::vector<bloc::Value> t;
    t.push_back(bloc::Value(new bloc::Literal(PQfname(desc, i))));
    switch (PQftype(desc, i))
    {
    case OIDBOOL:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_BOOLEAN)));
      break;
    case OIDINT2:
    case OIDINT4:
    case OIDINT8:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_INTEGER)));
      break;
    case OIDFLOAT4:
    case OIDFLOAT8:
    case OIDCASH:
    case OIDNUMERIC:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_NUMERIC)));
      break;
    case OIDCHAR:
    case OIDNAME:
    case OIDTEXT:
    case OIDBPCHAR:
    case OIDVARCHAR:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_LITERAL)));
      break;
    case OIDDATE:
    case OIDTIME:
    case OIDTIMESTAMP:
    case OIDTIMESTAMPTZ:
    case OIDTIMETZ:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_LITERAL)));
      break;
    case OIDCIDR:
    case OIDMACADDR8:
    case OIDMACADDR:
    case OIDINET:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_LITERAL)));
      break;
    case OIDBYTEA:
    case OIDBIT:
    case OIDVARBIT:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_TABCHAR)));
      break;
    case OIDJSON:
    case OIDXML:
    case OIDUUID:
    case OIDJSONB:
    case OIDJSONPATH:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_LITERAL)));
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
  PQclear(desc);
  return 1;
}

int PostgreSQL::Handle::fetch(bloc::Tuple ** row)
{
  if (!_stmt_async)
    throw RuntimeError(EXC_RT_OTHER_S, "No query in progress");

  PGresult * res = PQgetResult(_db);
  if (!res)
    throw RuntimeError(EXC_RT_USER_S, PQerrorMessage(_db));

  ExecStatusType s = PQresultStatus(res);
  if (s != PGRES_SINGLE_TUPLE && s != PGRES_TUPLES_OK)
  {
    _errmsg.assign(PQresultErrorMessage(res));
    PQclear(res);
    cancel();
    _stmt_async = false;
    throw RuntimeError(EXC_RT_USER_S, _errmsg.c_str());
  }

  /* initialize bindinds */
  if (_stmt_rs.empty())
  {
    /* Get total columns in the query */
    int column_count = PQnfields(res);
    /* initialize bindinds */
    _stmt_rs.resize(column_count);
    _stmt_decl.resize(column_count);

    /* fetch metadata of the result set */
    int i = 0;
    for (PG_BIND& bind : _stmt_rs())
    {
      bind.buffer_type = PQftype(res, i);
      bind.buffer_length = PQfsize(res, i);
      bind.name = PQfname(res, i);
      switch (bind.buffer_type)
      {
      case OIDBOOL:
        _stmt_decl[i] = bloc::Type::BOOLEAN;
        break;
      case OIDINT2:
      case OIDINT4:
      case OIDINT8:
        _stmt_decl[i] = bloc::Type::INTEGER;
        break;
      case OIDFLOAT4:
      case OIDFLOAT8:
      case OIDCASH:
      case OIDNUMERIC:
        _stmt_decl[i] = bloc::Type::NUMERIC;
        break;
      case OIDCHAR:
      case OIDNAME:
      case OIDTEXT:
      case OIDBPCHAR:
      case OIDVARCHAR:
        _stmt_decl[i] = bloc::Type::LITERAL;
        break;
      case OIDDATE:
      case OIDTIME:
      case OIDTIMESTAMP:
      case OIDTIMESTAMPTZ:
      case OIDTIMETZ:
        _stmt_decl[i] = bloc::Type::LITERAL;
        break;
      case OIDCIDR:
      case OIDMACADDR8:
      case OIDMACADDR:
      case OIDINET:
        _stmt_decl[i] = bloc::Type::LITERAL;
        break;
      case OIDBYTEA:
      case OIDBIT:
      case OIDVARBIT:
        _stmt_decl[i] = bloc::Type::TABCHAR;
        break;
      case OIDJSON:
      case OIDXML:
      case OIDUUID:
      case OIDJSONB:
      case OIDJSONPATH:
        _stmt_decl[i] = bloc::Type::LITERAL;
        break;
      default:
        /* use complex for unsupported datatype */
        _stmt_decl[i] = bloc::Type::COMPLEX;
      }
      ++i;
    }
  }

  /* end of stream */
  if (s != PGRES_SINGLE_TUPLE)
  {
    PQclear(res);
    /* drain the null*/
    (void)PQgetResult(_db);
    _stmt_async = false;
    return 0;
  }

  /* load row data */
  std::vector<bloc::Value> t;
  int i = 0, r = 1;
  for (PG_BIND& bind : _stmt_rs())
  {
    int vnull = PQgetisnull(res, 0, i);
    int vsize = PQgetlength(res, 0, i);
    char * vdata = PQgetvalue(res, 0, i);

    switch (bind.buffer_type)
    {
    case OIDBOOL:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_boolean));
      else
        t.push_back(bloc::Value(bloc::Bool(vdata[0] == 't' ? true : false)));
      break;
    case OIDINT2:
    case OIDINT4:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_integer));
      else
        t.push_back(bloc::Value(bloc::Integer(std::stoi(vdata))));
      break;
    case OIDINT8:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_integer));
      else
        t.push_back(bloc::Value(bloc::Integer(std::stoll(vdata))));
      break;
    case OIDFLOAT4:
    case OIDFLOAT8:
    case OIDCASH:
    case OIDNUMERIC:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_numeric));
      else
        t.push_back(bloc::Value(bloc::Numeric(std::stod(vdata))));
      break;
    case OIDCHAR:
    case OIDNAME:
    case OIDTEXT:
    case OIDBPCHAR:
    case OIDVARCHAR:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_literal));
      else
        t.push_back(bloc::Value(new bloc::Literal(vdata)));
      break;
    case OIDBYTEA:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_tabchar));
      else
      {
        size_t blen;
        unsigned char * braw = PQunescapeBytea((unsigned char*)vdata, &blen);
        if (!braw)
          t.push_back(bloc::Value(bloc::Value::type_tabchar));
        else
        {
          t.push_back(bloc::Value(new bloc::TabChar(braw, braw + blen)));
          PQfreemem(braw);
        }
      }
      break;
    case OIDDATE:
    case OIDTIME:
    case OIDTIMESTAMP:
    case OIDTIMESTAMPTZ:
    case OIDTIMETZ:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_literal));
      else
        t.push_back(bloc::Value(new bloc::Literal(vdata)));
      break;
    case OIDBIT:
    case OIDVARBIT:
    case OIDCIDR:
    case OIDMACADDR8:
    case OIDMACADDR:
    case OIDINET:
    case OIDJSON:
    case OIDXML:
    case OIDUUID:
    case OIDJSONB:
    case OIDJSONPATH:
      if (vnull)
        t.push_back(bloc::Value(bloc::Value::type_literal));
      else
        t.push_back(bloc::Value(new bloc::Literal(vdata)));
      break;
    default:
      /* use complex for unsupported datatype */
      t.push_back(bloc::Value(bloc::Value::type_complex));
    }
    ++i;
  }

  PQclear(res);
  *row = new bloc::Tuple(std::move(t));

  return 1;
}

int PostgreSQL::Handle::finalize()
{
  if (_stmt_async)
    cancel();
  _stmt_async = false;
  _stmt_prepared = false;
  _stmt_rs.clear();
  return 1;
}

void PostgreSQL::Handle::cancel()
{
  PGcancel* cancel = PQgetCancel(_db);
  if (!cancel)
    bloc::RuntimeError(EXC_RT_USER_S, "Failed to cancel");

  char errbuf[256];
  int r = PQcancel(cancel, errbuf, sizeof(errbuf));
  PQfreeCancel(cancel);

  if (!r)
    throw bloc::RuntimeError(EXC_RT_USER_S, errbuf);

  /* drain any rows already in transit */
  PGresult* res = nullptr;
  while ((res = PQgetResult(_db)) != nullptr)
    PQclear(res);
}

} /* namespace import */
} /* namespace bloc */
