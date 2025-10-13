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

#include "plugin_oracle.h"
#include "oracle/odpi/include/dpi.h"
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
 * Create the module OracleImport
 */
PLUGINCREATOR(OraclePlugin)

namespace bloc
{
namespace plugin
{
namespace Oracle
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "O", 0 }, // oracle
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // connect string
  { "L", 0 }, // user
  { "L", 0 }, // password
};

static PLUGIN_TYPE ctor_2_args[]  = {
  { "L", 0 }, // connect string
  { "L", 0 }, // user
  { "L", 0 }, // password
  { "L", 0 }, // auth mode
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Clone a database connection.\n"
          "The current session will be shared with the new clone." },
  { 1,      3,  ctor_1_args,
          "Opening a new database connection.\n"
  "1  : connect string\n2  : user\n3  : password" },
  { 2,      4,  ctor_2_args,
          "Opening a new database connection with auth mode.\n"
  "1  : connect string\n2  : user\n3  : password\n"
  "4  : auth mode ( SYSDBA | SYSOPER | SYSASM )"},
};

enum Method
{
  Open1 = 0, Open2, Close, IsOpen, Commit, Rollback, Query1, Query2,
  Exec1, Exec2, ErrMsg, Prepare, Execute1, Execute2, Header, Fetch, Finalize,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG connect1_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // connect string
  { PLUGIN_IN,    { "L", 0 } }, // user
  { PLUGIN_IN,    { "L", 0 } }, // password
};

static PLUGIN_ARG connect2_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // connect string
  { PLUGIN_IN,    { "L", 0 } }, // user
  { PLUGIN_IN,    { "L", 0 } }, // password
  { PLUGIN_IN,    { "L", 0 } }, // auth mode
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
  { Open1,   "open",      { "B", 0 },     3, connect1_args,
          "Opening a new database connection.\n"
  "1  : connect string\n2  : user\n3  : password" },
  { Open2,   "open",      { "B", 0 },     4, connect2_args,
          "Opening a new database connection with auth mode.\n"
  "1  : connect string\n2  : user\n3  : password\n"
  "4  : auth mode ( SYSDBA | SYSOPER | SYSASM )" },
  { Close,   "close",     { "B", 0 },     0, nullptr,
          "Closing a database connection." },
  { IsOpen,   "isopen",   { "B", 0 },     0, nullptr,
          "Check a database connection is open." },
  { Commit,   "commit",     { "B", 0 },     0, nullptr,
          "Commits the current active transaction." },
  { Rollback, "rollback",   { "B", 0 },     0, nullptr,
          "Rolls back the current active transaction." },
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
static dpiContext * gContext = nullptr;

typedef struct {
  std::string connectString;
  std::string user;
  std::string password;
  std::string authMode;
} ORACLE_PARAMS;

struct Handle {
  ORACLE_PARAMS _params;
  dpiContext * _context = nullptr;
  dpiConn * _conn = nullptr;
  std::string _errmsg;
  dpiStmt * _stmt = nullptr;

  ~Handle()
  {
    close();
    _context = nullptr;
  }
  Handle() { }
  const char * errmsg() const { return _errmsg.c_str(); }
  int initContext();
  int onError();
  int open(const std::string& connectString,
           const std::string& user,
           const std::string& password,
           const std::string& authMode);
  int close();
  int isOpen() const { return (_conn ? 1 : 0); }
  int autoCommit(bool mode);
  int fetchall(bloc::Collection ** rs);
  int query(const std::string& str, bloc::Collection ** rs);
  int query(const std::string& str, bloc::Tuple& args, bloc::Collection ** rs);
  int exec(const std::string& str);
  int exec(const std::string& str, bloc::Tuple& args);
  int prepare(const std::string& str);
  int execute(bloc::Tuple& args);
  int execute();
  int fetchMetadata(unsigned numcol);
  int header(bloc::Collection ** hd);
  int fetch(bloc::Tuple ** row, bool& found);
  int commit();
  int rollback();
  int finalize();

  struct BindVar {
    dpiOracleTypeNum otype = DPI_ORACLE_TYPE_NONE;
    dpiNativeTypeNum ntype = DPI_NATIVE_TYPE_NULL;
    dpiVar * var = nullptr;
    dpiData * data = nullptr;
  };

  class Bindings {
    std::vector<BindVar> _bindings;
  public:
    Bindings() { }
    explicit Bindings(unsigned size);
    explicit Bindings(const Bindings&) = delete;
    Bindings& operator=(const Bindings&) = delete;
    ~Bindings();
    void clear();
    void reset(unsigned size);
    bool empty() const { return _bindings.empty(); }
    unsigned size() const { return _bindings.size(); }
    std::vector<BindVar>& operator()() { return _bindings; }
  };

  std::vector<bloc::Literal> _stmt_columns;
  bloc::TupleDecl::Decl _stmt_decl;

  int bind_args(Bindings& bindings, bloc::Tuple& args);
  int read_blob(dpiLob * lob, bloc::TabChar ** raw);
  int read_clob(dpiLob * lob, bloc::Literal ** str);
};

} /* namespace Oracle */

OraclePlugin::~OraclePlugin()
{
  /* clean up and free resources used by the library */
  if (Oracle::gContext)
    dpiContext_destroy(Oracle::gContext);
  Oracle::gContext = nullptr;
}

void OraclePlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "oracle";
  interface->method_count = sizeof(Oracle::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = Oracle::methods;
  interface->ctors_count = sizeof(Oracle::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = Oracle::ctors;
}

void * OraclePlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  std::string msg;
  switch (ctor_id)
  {
  case 0: /* copy ctor */
  {
    Oracle::Handle * h = nullptr;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull() || (h = static_cast<Oracle::Handle*>(a0.complex()->instance())) == nullptr)
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    Oracle::Handle * n = new Oracle::Handle();
    if (n->initContext())
    {
      n->_params = h->_params;
      if (h->_conn)
      {
        /* share the connection */
        if (dpiConn_addRef(h->_conn) == 0)
        {
          n->_conn = h->_conn;
          return n;
        }
      }
      else
        return n;
    }
    msg.assign(n->errmsg());
    delete n;
    break;
  }

  case 1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    Oracle::Handle * h = new Oracle::Handle();
    if (h->initContext() && h->open(*a0.literal(), *a1.literal(), *a2.literal(), ""))
      return h;
    msg.assign(h->errmsg());
    delete h;
    break;
  }

  case 2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    Oracle::Handle * h = new Oracle::Handle();
    if (h->initContext() && h->open(*a0.literal(), *a1.literal(), *a2.literal(),
                (a3.isNull() ? "" : *a3.literal())))
      return h;
    msg.assign(h->errmsg());
    delete h;
    break;
  }

  default: /* default ctor */
  {
    Oracle::Handle * h = new Oracle::Handle();
    if (h->initContext())
      return h;
    msg.assign(h->errmsg());
    delete h;
    break;
  }
  }

  if (!msg.empty())
    throw RuntimeError(EXC_RT_USER_S, msg.c_str());
  return nullptr;
}

void OraclePlugin::destroyObject(void * object)
{
  Oracle::Handle * h = static_cast<Oracle::Handle*>(object);
  delete h;
}

bloc::Value * OraclePlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  Oracle::Handle * h = static_cast<Oracle::Handle*>(object_this.instance());

  switch (method_id)
  {
  case Oracle::Open1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (h->isOpen() == 1)
      h->close();
    return new bloc::Value(bloc::Bool(h->open(*a0.literal(),
                                              *a1.literal(),
                                              *a2.literal(),
                                              "")));
  }

  case Oracle::Open2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (h->isOpen() == 1)
      h->close();
    return new bloc::Value(bloc::Bool(h->open(*a0.literal(),
                                              *a1.literal(),
                                              *a2.literal(),
                                              (a3.isNull() ? "" : *a3.literal()))));
  }

  case Oracle::Close:
    if (h->isOpen())
      return new bloc::Value(bloc::Bool(h->close()));
    return new bloc::Value(bloc::Bool(0));

  case Oracle::IsOpen:
    return new bloc::Value(bloc::Bool(h->isOpen()));

  default:
    if (!h->isOpen())
      throw RuntimeError(EXC_RT_OTHER_S, "Database Connection not open.");
  }

  switch (method_id)
  {

  case Oracle::Commit:
  {
    if (!h->commit())
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case Oracle::Rollback:
  {
    if (!h->rollback())
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case Oracle::Query1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection * c = nullptr;
    if (!h->query(*a0.literal(), &c))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(c);
  }

  case Oracle::Query2:
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

  case Oracle::Exec1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->exec(*a0.literal()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case Oracle::Exec2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->exec(*a0.literal(), *a1.tuple()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case Oracle::ErrMsg:
    return new bloc::Value(new bloc::Literal(std::string(h->errmsg())));

   case Oracle::Prepare:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->prepare(*a0.literal()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case Oracle::Execute1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!h->execute(*a0.tuple()))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));
  }

  case Oracle::Execute2:
    if (!h->execute())
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    return new bloc::Value(bloc::Bool(true));

  case Oracle::Header:
  {
    bloc::Collection * c = nullptr;
    int r = h->header(&c);
    return new bloc::Value(c);
  }

  case Oracle::Fetch:
  {
    // INOUT //
    if (!args[0]->symbol())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Tuple * t = nullptr;
    bool found;
    if (!h->fetch(&t, found))
      throw RuntimeError(EXC_RT_USER_S, h->errmsg());
    if (found)
      ctx.storeVariable(*args[0]->symbol(), bloc::Value(t));
    return new bloc::Value(bloc::Bool(found));
  }

  case Oracle::Finalize:
    return new bloc::Value(bloc::Bool(h->finalize()));

  default:
    break;
  }
  return nullptr;
}

Oracle::Handle::Bindings::Bindings(unsigned size)
{
  reset(size);
}

Oracle::Handle::Bindings::~Bindings()
{
  clear();
}

void Oracle::Handle::Bindings::reset(unsigned size)
{
  clear();
  _bindings.resize(size);
}

void Oracle::Handle::Bindings::clear()
{
  for (BindVar& bind : _bindings)
  {
    if (bind.var)
      dpiVar_release(bind.var);
    bind.var = nullptr;
    bind.data = nullptr;
    bind.otype = DPI_ORACLE_TYPE_NONE;
    bind.ntype = DPI_NATIVE_TYPE_NULL;
  }
}

int Oracle::Handle::bind_args(Bindings& bindings, bloc::Tuple& args)
{
  int i = 0;
  for (BindVar& bind : bindings())
  {
    if (args.size() > i)
    {
      bloc::Value& v = args[i++];

      switch (v.type().major())
      {
      case Type::BOOLEAN:
        bind.otype = DPI_ORACLE_TYPE_BOOLEAN;
        bind.ntype = DPI_NATIVE_TYPE_BOOLEAN;
        if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                           1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
          return onError();
        if (v.isNull())
          dpiData_setNull(bind.data);
        else
          dpiData_setBool(bind.data, (*v.boolean() ? 1 : 0));
        break;
      case Type::INTEGER:
        bind.otype = DPI_ORACLE_TYPE_NUMBER;
        bind.ntype = DPI_NATIVE_TYPE_INT64;
        if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                           1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
          return onError();
        if (v.isNull())
          dpiData_setNull(bind.data);
        else
          dpiData_setInt64(bind.data, *v.integer());
        break;
      case Type::NUMERIC:
        bind.otype = DPI_ORACLE_TYPE_NUMBER;
        bind.ntype = DPI_NATIVE_TYPE_DOUBLE;
        if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                           1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
          return onError();
        if (v.isNull())
          dpiData_setNull(bind.data);
        else
          dpiData_setDouble(bind.data, *v.numeric());
        break;
      case Type::LITERAL:
        bind.otype = DPI_ORACLE_TYPE_VARCHAR;
        bind.ntype = DPI_NATIVE_TYPE_BYTES;
        if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                           1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
          return onError();
        if (v.isNull())
          dpiData_setNull(bind.data);
        else
          dpiData_setBytes(bind.data, const_cast<char*>(v.literal()->c_str()), v.literal()->size());
        break;
      case Type::TABCHAR:
        bind.otype = DPI_ORACLE_TYPE_LONG_RAW;
        bind.ntype = DPI_NATIVE_TYPE_BYTES;
        if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                           1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
          return onError();
        if (v.isNull())
          dpiData_setNull(bind.data);
        else
          dpiVar_setFromBytes(bind.var,0, const_cast<char*>(v.tabchar()->data()), v.tabchar()->size());
        break;
      default:
        /* a null string */
        bind.otype = DPI_ORACLE_TYPE_VARCHAR;
        bind.ntype = DPI_NATIVE_TYPE_BYTES;
        if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                           1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
          return onError();
        dpiData_setNull(bind.data);
        break;
      }
    }
    else
    {
      /* a null string */
      bind.otype = DPI_ORACLE_TYPE_VARCHAR;
      bind.ntype = DPI_NATIVE_TYPE_BYTES;
      if (dpiConn_newVar(_conn, bind.otype, bind.ntype,
                         1, 0, 0, 0, nullptr, &bind.var, &bind.data) < 0)
        return onError();
      dpiData_setNull(bind.data);
    }
  }
  return 1;
}

int Oracle::Handle::initContext()
{
  if (!Oracle::gContext)
  {
    dpiErrorInfo info;
    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION,
                                    nullptr, &Oracle::gContext, &info) < 0)
    {
      _errmsg.assign(info.message, info.messageLength);
      return 0;
    }
  }
  _context = Oracle::gContext;
  return 1;
}

int Oracle::Handle::onError()
{
  dpiErrorInfo info;
  dpiContext_getError(_context, &info);
  _errmsg.assign(info.message, info.messageLength);
  return 0; // always return a failure
}

int Oracle::Handle::open(const std::string& connectString,
                         const std::string& user,
                         const std::string& password,
                         const std::string& authMode)
{
  _params.connectString = connectString;
  _params.user = user;
  _params.password = password;
  _params.authMode = authMode;

  dpiConnCreateParams connParams;
  // populate connection params
  if (dpiContext_initConnCreateParams(_context, &connParams) < 0)
    return onError();
  std::transform(_params.authMode.begin(), _params.authMode.end(),
                 _params.authMode.begin(), ::toupper);
  if (_params.authMode.empty())
    connParams.authMode = DPI_MODE_AUTH_DEFAULT;
  else if (_params.authMode == "SYSDBA")
    connParams.authMode = DPI_MODE_AUTH_SYSDBA;
  else if (_params.authMode == "SYSOPER")
    connParams.authMode = DPI_MODE_AUTH_SYSOPER;
  else if (_params.authMode == "SYSASM")
    connParams.authMode = DPI_MODE_AUTH_SYSASM;
  else
  {
    _errmsg.assign("Auth mode is incorrecly specified or invalid.");
    return 0;
  }

  // connect to the database
  if (dpiConn_create(_context,
                     _params.user.c_str(),
                     _params.user.size(),
                     _params.password.c_str(),
                     _params.password.size(),
                     _params.connectString.c_str(),
                     _params.connectString.size(),
                     NULL, &connParams, &_conn) < 0)
    return onError();

  return 1;
}

int Oracle::Handle::fetchall(bloc::Collection ** rs)
{
  bloc::Collection::container_t c;
  bloc::Tuple * row = nullptr;
  bool found;
  for (;;)
  {
    if (!fetch(&row, found))
      return 0;
    if (found)
      c.push_back(bloc::Value(row));
    else
      break;
  }
  *rs = new bloc::Collection(_stmt_decl, 1, std::move(c));
  return 1;
}

int Oracle::Handle::close()
{
  if (!_conn)
    return 0;
  finalize();
  dpiConn_release(_conn);
  _conn = nullptr;
  return 1;
}

int Oracle::Handle::query(const std::string& str, bloc::Collection ** rs)
{
  if (!prepare(str))
    return 0;
  if (!execute())
    return 0;
  if (!fetchall(rs))
    return 0;
  if (!finalize())
    return 0;
  return 1;
}

int Oracle::Handle::query(const std::string& str, bloc::Tuple& args, bloc::Collection ** rs)
{
  if (!prepare(str))
    return 0;
  if (!execute(args))
    return 0;
  if (!fetchall(rs))
    return 0;
  if (!finalize())
    return 0;
  return 1;
}

int Oracle::Handle::exec(const std::string& str)
{
  if (!prepare(str))
    return 0;
  if (!execute())
    return 0;
  if (!finalize())
    return 0;
  return 1;
}

int Oracle::Handle::exec(const std::string& str, bloc::Tuple& args)
{
  if (!prepare(str))
    return 0;
  if (!execute(args))
    return 0;
  if (!finalize())
    return 0;
  return 1;
}

int Oracle::Handle::prepare(const std::string& str)
{
  if (_stmt)
    finalize();
  if (dpiConn_prepareStmt(_conn, 0, str.c_str(), str.size(), NULL, 0, &_stmt) < 0)
    return onError();
  return 1;
}

int Oracle::Handle::execute(bloc::Tuple& args)
{
  if (_stmt == nullptr)
    throw RuntimeError(EXC_RT_OTHER_S, "No prepared statement.");
  /* bind arguments */
  uint32_t bc = 0;
  if (dpiStmt_getBindCount(_stmt, &bc) < 0)
    return onError();
  if (bc > args.size())
  {
    _errmsg.assign("Not all parameters bound.");
    return 0;
  }
  Bindings bindings(bc);
  if (!bind_args(bindings, args))
    return 0;
  int pos = 0;
  for (BindVar& bind : bindings())
  {
    /* use var features for raw data */
    if (bind.otype == DPI_ORACLE_TYPE_LONG_RAW)
    {
      if (dpiStmt_bindByPos(_stmt, ++pos, bind.var) < 0)
        return onError();
    }
    else
    {
      if (dpiStmt_bindValueByPos(_stmt, ++pos, bind.ntype, bind.data) < 0)
        return onError();
    }
  }
  /* execute */
  uint32_t numcol;
  if (dpiStmt_execute(_stmt, 0, &numcol) < 0)
    return onError();

  /* initialize metadata */
  return fetchMetadata(numcol);
}

int Oracle::Handle::execute()
{
  if (_stmt == nullptr)
    throw RuntimeError(EXC_RT_OTHER_S, "No prepared statement.");
  /* check bindings */
  uint32_t bc = 0;
  if (dpiStmt_getBindCount(_stmt, &bc) < 0)
    return onError();
  if (bc != 0)
  {
    _errmsg.assign("Not all parameters bound.");
    return 0;
  }
  /* execute */
  uint32_t numcol;
  if (dpiStmt_execute(_stmt, 0, &numcol) < 0)
    return onError();

  /* initialize metadata */
  return fetchMetadata(numcol);
}

int Oracle::Handle::fetchMetadata(unsigned numcol)
{
  /* fetch result set meta information */
  _stmt_decl.resize(numcol);
  _stmt_columns.reserve(numcol);
  _stmt_columns.clear();

  unsigned pos = 0;
  for (auto& decl : _stmt_decl)
  {
    dpiQueryInfo info;
    if (dpiStmt_getQueryInfo(_stmt, ++pos, &info) < 0)
      return onError();

    /* fill columns */
    _stmt_columns.push_back(bloc::Literal(info.name, info.nameLength));

    /* configure declarations */
    switch (info.typeInfo.oracleTypeNum)
    {
    case DPI_ORACLE_TYPE_BOOLEAN:
      decl = bloc::Type::BOOLEAN;
      break;
    case DPI_ORACLE_TYPE_NUMBER:
    case DPI_ORACLE_TYPE_NATIVE_DOUBLE:
    case DPI_ORACLE_TYPE_NATIVE_FLOAT:
      decl = bloc::Type::NUMERIC;
      break;
    case DPI_ORACLE_TYPE_NATIVE_INT:
    case DPI_ORACLE_TYPE_NATIVE_UINT:
      decl = bloc::Type::INTEGER;
      break;
    case DPI_ORACLE_TYPE_DATE:
    case DPI_ORACLE_TYPE_TIMESTAMP:
    case DPI_ORACLE_TYPE_TIMESTAMP_LTZ:
    case DPI_ORACLE_TYPE_TIMESTAMP_TZ:
      decl = bloc::Type::LITERAL;
      break;
    case DPI_ORACLE_TYPE_VARCHAR:
    case DPI_ORACLE_TYPE_CHAR:
    case DPI_ORACLE_TYPE_ROWID:
    case DPI_ORACLE_TYPE_CLOB:
    case DPI_ORACLE_TYPE_LONG_VARCHAR:
      decl = bloc::Type::LITERAL;
      break;
    case DPI_ORACLE_TYPE_RAW:
    case DPI_ORACLE_TYPE_NVARCHAR:
    case DPI_ORACLE_TYPE_NCHAR:
    case DPI_ORACLE_TYPE_NCLOB:
    case DPI_ORACLE_TYPE_BLOB:
    case DPI_ORACLE_TYPE_LONG_NVARCHAR:
    case DPI_ORACLE_TYPE_LONG_RAW:
      decl = bloc::Type::TABCHAR;
      break;
    default:
      /* use complex for unsupported datatype */
      decl = bloc::Type::COMPLEX;
    }
  }
  return 1;
}

int Oracle::Handle::header(bloc::Collection ** hd)
{
  if (_stmt == nullptr || _stmt_decl.empty())
    throw RuntimeError(EXC_RT_OTHER_S, "No query in progress.");

  unsigned col = 0;
  for (auto& decl : _stmt_decl)
  {
    std::vector<bloc::Value> t;
    t.push_back(bloc::Value(new bloc::Literal(_stmt_columns[col++])));
    switch (decl.major())
    {
    case bloc::Type::BOOLEAN:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_BOOLEAN)));
      break;
    case bloc::Type::INTEGER:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_INTEGER)));
      break;
    case bloc::Type::NUMERIC:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_NUMERIC)));
      break;
    case bloc::Type::LITERAL:
      t.push_back(bloc::Value(new bloc::Literal(bloc::Type::STR_LITERAL)));
      break;
    case bloc::Type::TABCHAR:
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
  return 1;
}

int Oracle::Handle::fetch(bloc::Tuple ** row, bool& found)
{
  if (_stmt == nullptr || _stmt_decl.empty())
    throw RuntimeError(EXC_RT_OTHER_S, "No query in progress.");

  int _found = 0;
  uint32_t bufferRowIndex = 0;
  if (dpiStmt_fetch(_stmt, &_found, &bufferRowIndex) < 0)
    return onError();
  if (!_found)
  {
    found = false;
    return 1;
  }
  found = true;

  std::vector<bloc::Value> t;
  unsigned pos = 0;
  for (auto& decl : _stmt_decl)
  {
    dpiData * data;
    dpiNativeTypeNum ntype;
    if (dpiStmt_getQueryValue(_stmt, ++pos, &ntype, &data) < 0)
      return onError();

    if (data->isNull)
    {
      t.push_back(bloc::Value(decl));
      continue;
    }

    switch (ntype)
    {
    case DPI_NATIVE_TYPE_BOOLEAN:
      t.push_back(bloc::Value(bloc::Bool(data->value.asBoolean ? true : false)));
      break;
    case DPI_NATIVE_TYPE_INT64:
      t.push_back(bloc::Value(bloc::Integer(data->value.asInt64)));
      break;
    case DPI_NATIVE_TYPE_UINT64:
      t.push_back(bloc::Value(bloc::Numeric(data->value.asUint64)));
      break;
    case DPI_NATIVE_TYPE_DOUBLE:
      t.push_back(bloc::Value(bloc::Numeric(data->value.asDouble)));
      break;
    case DPI_NATIVE_TYPE_FLOAT:
      t.push_back(bloc::Value(bloc::Numeric(data->value.asFloat)));
      break;
    case DPI_NATIVE_TYPE_ROWID:
      {
        const char * str;
        uint32_t len;
        if (dpiRowid_getStringValue(data->value.asRowid, &str, &len) < 0)
            return onError();
        t.push_back(bloc::Value(new bloc::Literal(str, len)));
      }
      break;
    case DPI_NATIVE_TYPE_BYTES:
      if (decl == bloc::Type::LITERAL)
        t.push_back(bloc::Value(new bloc::Literal(data->value.asBytes.ptr,
                data->value.asBytes.length)));
      else
        t.push_back(bloc::Value(new bloc::TabChar(data->value.asBytes.ptr,
                data->value.asBytes.ptr + data->value.asBytes.length)));
      break;
    case DPI_NATIVE_TYPE_TIMESTAMP:
      {
        char buf[48];
        if (data->value.asTimestamp.fsecond == 0)
        {
          if (data->value.asTimestamp.tzHourOffset == 0 &&
                  data->value.asTimestamp.tzMinuteOffset == 0)
            snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02dZ",
                    data->value.asTimestamp.year,
                    data->value.asTimestamp.month,
                    data->value.asTimestamp.day,
                    data->value.asTimestamp.hour,
                    data->value.asTimestamp.minute,
                    data->value.asTimestamp.second);
          else
            snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d",
                    data->value.asTimestamp.year,
                    data->value.asTimestamp.month,
                    data->value.asTimestamp.day,
                    data->value.asTimestamp.hour,
                    data->value.asTimestamp.minute,
                    data->value.asTimestamp.second,
                    data->value.asTimestamp.tzHourOffset,
                    data->value.asTimestamp.tzMinuteOffset);
        }
        else
        {
          if (data->value.asTimestamp.tzHourOffset == 0 &&
                  data->value.asTimestamp.tzMinuteOffset == 0)
            snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%06uZ",
                    data->value.asTimestamp.year,
                    data->value.asTimestamp.month,
                    data->value.asTimestamp.day,
                    data->value.asTimestamp.hour,
                    data->value.asTimestamp.minute,
                    data->value.asTimestamp.second,
                    data->value.asTimestamp.fsecond);
          else
            snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%06u%+03d:%02d",
                    data->value.asTimestamp.year,
                    data->value.asTimestamp.month,
                    data->value.asTimestamp.day,
                    data->value.asTimestamp.hour,
                    data->value.asTimestamp.minute,
                    data->value.asTimestamp.second,
                    data->value.asTimestamp.fsecond,
                    data->value.asTimestamp.tzHourOffset,
                    data->value.asTimestamp.tzMinuteOffset);
        }
        t.push_back(bloc::Value(new bloc::Literal(buf)));
      }
      break;
    case DPI_NATIVE_TYPE_LOB:
      if (decl == bloc::Type::LITERAL)
      {
        bloc::Literal * str = nullptr;
        if (!read_clob(data->value.asLOB, &str))
          return 0;
        t.push_back(bloc::Value(str));
      }
      else
      {
        bloc::TabChar * raw = nullptr;
        if (!read_blob(data->value.asLOB, &raw))
          return 0;
        t.push_back(bloc::Value(raw));
      }
      break;
    case DPI_NATIVE_TYPE_NULL:
    default:
      t.push_back(bloc::Value(decl));
    }
  }
  *row = new bloc::Tuple(std::move(t));
  return 1;
}

int Oracle::Handle::commit()
{
  if (!_conn)
    return 0;
  if (dpiConn_commit(_conn) < 0)
    return onError();
  return 1;
}

int Oracle::Handle::rollback()
{
  if (!_conn)
    return 0;
  if (dpiConn_rollback(_conn) < 0)
    return onError();
  return 1;
}

int Oracle::Handle::finalize()
{
  if (!_stmt)
    return 0;
  dpiStmt_release(_stmt);
  _stmt = nullptr;
  _stmt_decl.clear();
  _stmt_columns.clear();
  return 1;
}

#define LOB_CHUNCK_SIZE 2000

int Oracle::Handle::read_blob(dpiLob * lob, bloc::TabChar ** raw)
{
  uint64_t len, bs;
  if (dpiLob_getSize(lob, &len) < 0)
    return onError();
  if (dpiLob_getBufferSize(lob, LOB_CHUNCK_SIZE, &bs) < 0)
    return onError();
  bloc::TabChar * tmp = new bloc::TabChar();
  if (len > 0)
  {
    tmp->reserve(len);
    char * buf = new char[bs];
    uint64_t offset = 1;
    while (offset <= len)
    {
      uint64_t count = bs;
      if (dpiLob_readBytes(lob, offset, LOB_CHUNCK_SIZE, buf, &count) < 0)
      {
        delete [] buf;
        delete tmp;
        return onError();
      }
      offset += LOB_CHUNCK_SIZE;
      tmp->insert(tmp->end(), buf, buf + count);
    }
    delete [] buf;
  }
  *raw = tmp;
  return 1;
}

int Oracle::Handle::read_clob(dpiLob * lob, bloc::Literal ** str)
{
  uint64_t len, bs;
  if (dpiLob_getSize(lob, &len) < 0)
    return onError();
  if (dpiLob_getBufferSize(lob, LOB_CHUNCK_SIZE, &bs) < 0)
    return onError();
  bloc::Literal * tmp = new bloc::Literal();
  if (len > 0)
  {
    tmp->reserve(len);
    char * buf = new char[bs];
    uint64_t offset = 1;
    while (offset <= len)
    {
      uint64_t count = bs;
      if (dpiLob_readBytes(lob, offset, LOB_CHUNCK_SIZE, buf, &count) < 0)
      {
        delete [] buf;
        delete tmp;
        return onError();
      }
      offset += LOB_CHUNCK_SIZE;
      tmp->insert(tmp->end(), buf, buf + count);
    }
    delete [] buf;
  }
  *str = tmp;
  return 1;
}

} /* namespace import */
} /* namespace bloc */
