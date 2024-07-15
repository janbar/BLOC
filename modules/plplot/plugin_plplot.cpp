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

#include "plugin_plplot.h"
#include "parseoptions.h"
#include "plplot/plstream.h"
#include <blocc/exception_runtime.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/debug.h>
#include <vector>
#include <map>
/*
 * Create the module PLPLOTImport
 */
PLUGINCREATOR(PLPLOTPlugin)

namespace bloc
{
namespace plugin
{
namespace PLPLOT
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "L", 0 }, // device name
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // device name
  { "L", 0 }, // output filename
};


static PLUGIN_CTOR ctors[] =
{
  { 0,      1,   ctor_0_args,
          "Create new plot for the given device name." },
  { 1,      2,   ctor_1_args,
          "Create new plot for the given device name and output file name." },
};

enum Method
{
  Usage = 0, Init, Init1, Close, Replot, Pause, Pause1,
  Env, Env1, Lab, Line, Adv, Vpor, Wind, Width, Col0, Col1, Colbga, Col0a,
  Ptex, Mtex, Box, Chr,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG init1_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // options
};

static PLUGIN_ARG pause_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // paused
};

static PLUGIN_ARG env_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
};

static PLUGIN_ARG env1_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
  { PLUGIN_IN,    { "I", 0 } }, // just
  { PLUGIN_IN,    { "I", 0 } }, // axis
};

static PLUGIN_ARG lab_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // xlabel
  { PLUGIN_IN,    { "L", 0 } }, // ylabel
  { PLUGIN_IN,    { "L", 0 } }, // tlabel
};

static PLUGIN_ARG line_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x points
  { PLUGIN_IN,    { "N", 1 } }, // y points
};

static PLUGIN_ARG adv_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // page
};

static PLUGIN_ARG vpor_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
};

static PLUGIN_ARG wind_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
};

static PLUGIN_ARG width_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // width
};

static PLUGIN_ARG col0_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // color
};

static PLUGIN_ARG col1_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // col1
};

static PLUGIN_ARG colbga_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // r
  { PLUGIN_IN,    { "I", 0 } }, // g
  { PLUGIN_IN,    { "I", 0 } }, // b
  { PLUGIN_IN,    { "N", 0 } }, // alpha
};

static PLUGIN_ARG col0a_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // ivol0
  { PLUGIN_IN,    { "I", 0 } }, // r
  { PLUGIN_IN,    { "I", 0 } }, // g
  { PLUGIN_IN,    { "I", 0 } }, // b
  { PLUGIN_IN,    { "N", 0 } }, // alpha
};

static PLUGIN_ARG ptex_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // x coordinate
  { PLUGIN_IN,    { "N", 0 } }, // y coordinate
  { PLUGIN_IN,    { "N", 0 } }, // dx
  { PLUGIN_IN,    { "N", 0 } }, // dy
  { PLUGIN_IN,    { "N", 0 } }, // just
  { PLUGIN_IN,    { "L", 0 } }, // text
};

static PLUGIN_ARG mtex_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // side
  { PLUGIN_IN,    { "N", 0 } }, // disp
  { PLUGIN_IN,    { "N", 0 } }, // pos
  { PLUGIN_IN,    { "N", 0 } }, // just
  { PLUGIN_IN,    { "L", 0 } }, // text
};

static PLUGIN_ARG box_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // xopy
  { PLUGIN_IN,    { "N", 0 } }, // xtick
  { PLUGIN_IN,    { "I", 0 } }, // nxsub
  { PLUGIN_IN,    { "L", 0 } }, // yopt
  { PLUGIN_IN,    { "N", 0 } }, // ytick
  { PLUGIN_IN,    { "I", 0 } }, // nysub
};

static PLUGIN_ARG chr_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // def
  { PLUGIN_IN,    { "N", 0 } }, // scale
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{

  { Usage,    "usage",      { "B", 0 },     0, nullptr,
          "Print usage and syntax of options." },
  { Init,     "init",       { "B", 0 },     0, nullptr,
          "Initialize the plotting session." },
  { Init1,    "init",       { "B", 0 },     1, init1_args,
          "Initialize the plotting session with the given options." },
  { Close,    "close",      { "B", 0 },     0, nullptr,
          "End plotting session." },
  { Replot,   "replot",     { "B", 0 },     0, nullptr,
          "Replays contents of plot buffer to current device/file." },
  { Pause,   "pause",       { "B", 0 },     1, pause_args,
          "Set the pause (on end-of-page) status. Default is off." },
  { Pause1,  "pause",       { "B", 0 },     0, nullptr,
          "Returns the pause status." },
  { Env,      "env",        { "B", 0 },     4, env_args,
          "Set up standard window and draw box." },
  { Env1,     "env",        { "B", 0 },     6, env1_args,
          "Set up standard window and draw box." },
  { Lab,      "lab",        { "B", 0 },     3, lab_args,
          "Simple routine to write labels." },
  { Line,     "line",       { "B", 0 },     2, line_args,
          "Draw a line." },
  { Adv,      "adv",        { "B", 0 },     1, adv_args,
          "Advance the (sub-)page." },
  { Vpor,     "vpor",       { "B", 0 },     4, vpor_args,
          "Specify viewport using normalized subpage coordinates." },
  { Wind,     "wind",       { "B", 0 },     4, wind_args,
          "Specify window." },
  { Width,    "width",      { "B", 0 },     1, width_args,
          "Set pen width." },
  { Col0,      "col0",      { "B", 0 },     1, col0_args,
          "Sets the color index for cmap0." },
  { Col1,      "col1",      { "B", 0 },     1, col1_args,
          "Sets the color index for cmap1." },
  { Colbga,    "colbga",    { "B", 0 },     4, colbga_args,
          "Set the background color by 8-bit RGB value and floating alpha transparency value." },
  { Col0a,     "col0a",     { "B", 0 },     5, col0a_args,
          "Set 8-bit RGB values and floating alpha transparency value for given cmap0 color index." },
  { Ptex,      "ptex",      { "B", 0 },     6, ptex_args,
          "Write text inside the viewport." },
  { Mtex,      "mtex",      { "B", 0 },     5, mtex_args,
          "Write text relative to viewport boundaries." },
  { Box,      "box",        { "B", 0 },     6, box_args,
          "Draw a box with axes, etc." },
  { Chr,      "chr",        { "B", 0 },     2, chr_args,
          "Set character size." },
};

/**
 * The state of handle
 */
struct Handle {
  plstream *_pls;
  bool _paused;
  int _nx;
  int _ny;
  std::string _device;
  std::string _filename;
  std::string _msg;

  typedef struct { PLINT r; PLINT g; PLINT b; PLFLT a; } rgba_t;
  std::map<PLINT, rgba_t> _palette;

  ~Handle()
  {
    if (_pls)
      delete _pls;
  }
  Handle(const std::string& device, const std::string& filename) :
  _pls(nullptr),
  _paused(false),
  _nx(1),
  _ny(1),
  _device(device),
  _filename(filename) { reset(); }
  void reset();
  bool init(const std::string& args);
  bool close();
  bool replot();
  bool paused(bool enabled);
  bool env(double xmin, double xmax, double ymin, double ymax, int just, int axis);
  bool lab(const std::string& xlabel, const std::string& ylabel, const std::string& tlabel);
  bool line(int n, const double * x, const double * y);
  bool adv(int page);
  bool vpor(double xmin, double xmax, double ymin, double ymax);
  bool wind(double xmin, double xmax, double ymin, double ymax);
  bool width(double width);
  bool col0(int color);
  bool col1(double col1);
  bool colbga(int r, int g, int b, double alpha);
  bool col0a(int col0, int r, int g, int b, double alpha);
  bool ptex(double x, double y, double dx, double dy, double just, const std::string& text);
  bool mtex(const std::string& side, double disp, double pos, double just, const std::string& text);
  bool box(const std::string& xopt, double xtick, int nxsub, const std::string& yopt, double ytick, int nysub);
  bool chr(double def, double scale);
};

} /* namespace PLPLOT */

void PLPLOTPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "plplot";
  interface->method_count = sizeof(PLPLOT::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = PLPLOT::methods;
  interface->ctors_count = sizeof(PLPLOT::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = PLPLOT::ctors;
}

void * PLPLOTPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  PLPLOT::Handle * h = nullptr;
  switch(ctor_id)
  {
  case 0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    h = new PLPLOT::Handle(*a0.literal(), std::string());
    break;
  }
  case 1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    h = new PLPLOT::Handle(*a0.literal(), *a1.literal());
    break;
  }

  default: /* default ctor */
    h = new PLPLOT::Handle(std::string(), std::string());
    break;
  }
  return h;
}

void PLPLOTPlugin::destroyObject(void * object)
{
  PLPLOT::Handle * h = static_cast<PLPLOT::Handle*>(object);
  delete h;
}

bloc::Value * PLPLOTPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  PLPLOT::Handle * h = static_cast<PLPLOT::Handle*>(object_this.instance());

  switch (method_id)
  {
  case PLPLOT::Usage:
    h->_pls->OptUsage();
    return new bloc::Value(bloc::Bool(true));
  case PLPLOT::Init:
    return new bloc::Value(bloc::Bool(h->init(std::string())));
  case PLPLOT::Init1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->init(*a0.literal())));
  }
  case PLPLOT::Close:
    return new bloc::Value(bloc::Bool(h->close()));
  case PLPLOT::Replot:
    return new bloc::Value(bloc::Bool(h->replot()));
  case PLPLOT::Pause:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->paused(*a0.boolean())));
  }
  case PLPLOT::Pause1:
  {
    return new bloc::Value(bloc::Bool(h->_paused));
  }
  case PLPLOT::Env:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->env(xmin, xmax, ymin, ymax, 0, 0)));
  }
  case PLPLOT::Env1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[2]->value(ctx);
    bloc::Value& a5 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull() || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    int just = (int) *a4.integer();
    int axis = (int) *a5.integer();
    return new bloc::Value(bloc::Bool(h->env(xmin, xmax, ymin, ymax, just, axis)));
  }
  case PLPLOT::Lab:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->lab(*a0.literal(), *a1.literal(), *a2.literal())));
  }
  case PLPLOT::Line:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    std::vector<double> vx;
    vx.reserve(n);
    std::vector<double> vy;
    vy.reserve(n);
    for(size_t i = 0; i < n; ++i)
    {
      bloc::Value& v0 = x.at(i);
      if (v0.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
      vx.push_back(*v0.numeric());
      bloc::Value& v1 = y.at(i);
      if (v1.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
      vy.push_back(*v1.numeric());
    }
    return new bloc::Value(bloc::Bool(h->line((int) n, &vx[0], &vy[0])));
  }
  case PLPLOT::Adv:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->adv((int) *a0.integer())));
  }
  case PLPLOT::Vpor:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->vpor(xmin, xmax, ymin, ymax)));
  }
  case PLPLOT::Wind:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->wind(xmin, xmax, ymin, ymax)));
  }
  case PLPLOT::Width:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double w = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    return new bloc::Value(bloc::Bool(h->width(w)));
  }
  case PLPLOT::Col0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->col0(*a0.integer())));
  }
  case PLPLOT::Col1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->col1(*a0.numeric())));
  }
  case PLPLOT::Colbga:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->colbga((int) *a0.integer(), (int) *a1.integer(), (int) *a2.integer(), *a3.numeric())));
  }
  case PLPLOT::Col0a:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->col0a((int) *a0.integer(), (int) *a1.integer(), (int) *a2.integer(), (int) *a3.integer(), *a4.numeric())));
  }
  case PLPLOT::Ptex:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
       || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double x = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double y = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double dx = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double dy = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    double just = (a4.type() == bloc::Type::NUMERIC ? *a4.numeric() : *a4.integer());
    return new bloc::Value(bloc::Bool(h->ptex(x, y, dx, dy, just, *a5.literal())));
  }

  case PLPLOT::Mtex:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double disp = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double pos = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double just = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->mtex(*a0.literal(), disp, pos, just, *a4.literal())));
  }

  case PLPLOT::Box:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
            || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xtick = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ytick = (a4.type() == bloc::Type::NUMERIC ? *a4.numeric() : *a4.integer());
    return new bloc::Value(bloc::Bool(h->box(*a0.literal(), xtick, (int) *a2.integer(), *a3.literal(), ytick, (int) *a5.integer())));
  }

  case PLPLOT::Chr:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double def = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double scale = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    return new bloc::Value(bloc::Bool(h->chr(def, scale)));
  }

  default:
    break;
  }
  return nullptr;
}

void PLPLOT::Handle::reset()
{
  close();
  _pls = new plstream((PLINT)_nx, (PLINT)_ny, _device.c_str(), _filename.c_str());
  _pls->spause(_paused);
  /* load customized colors */
  for(auto const& e : _palette)
    _pls->scol0a(e.first, e.second.r, e.second.g, e.second.b, e.second.a);
}

bool PLPLOT::Handle::init(const std::string& args)
{
  int failed = 0;
  std::vector<std::pair<std::string, std::string> > options;
  parseoptions(args, options);
  reset();
  for(auto& opt : options)
  {
    failed = (int) _pls->setopt(opt.first.c_str(), opt.second.c_str());
    if (failed != 0)
    {
      _msg.assign("Invalid option: ").append(opt.first).append(" = ").append(opt.second);
      return false;
    }
  }

  _pls->init();
  return true;
}

bool PLPLOT::Handle::close()
{
  if (_pls)
  {
    _pls->spause(0);
    delete _pls;
    _pls = nullptr;
  }
  return true;
}

bool PLPLOT::Handle::replot()
{
  _pls->replot();
  return true;
}

bool PLPLOT::Handle::paused(bool enabled)
{
  _pls->spause(enabled);
  _paused = enabled;
  return enabled;
}

bool PLPLOT::Handle::env(double xmin, double xmax, double ymin, double ymax, int just, int axis)
{
  _pls->env((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax, (PLINT) just, (PLINT) axis);
  return true;
}

bool PLPLOT::Handle::lab(const std::string& xlabel, const std::string& ylabel, const std::string& tlabel)
{
  _pls->lab(xlabel.c_str(), ylabel.c_str(), tlabel.c_str());
  return true;
}

bool PLPLOT::Handle::line(int n, const double * x, const double * y)
{
  _pls->line((PLINT) n, (const PLFLT*) x, (const PLFLT*) y);
  return true;
}

bool PLPLOT::Handle::adv(int page)
{
  _pls->adv((PLINT) page);
  return true;
}

bool PLPLOT::Handle::vpor(double xmin, double xmax, double ymin, double ymax)
{
  _pls->vpor((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax);
  return true;
}

bool PLPLOT::Handle::wind(double xmin, double xmax, double ymin, double ymax)
{
  _pls->wind((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax);
  return true;
}

bool PLPLOT::Handle::width(double width)
{
  _pls->width((PLFLT) width);
  return true;
}

bool PLPLOT::Handle::col0(int color)
{
  _pls->col0((PLINT) color);
  return true;
}

bool PLPLOT::Handle::col1(double col1)
{
  _pls->col1((PLFLT) col1);
  return true;
}

bool PLPLOT::Handle::colbga(int r, int g, int b, double alpha)
{
  _pls->scolbga((PLINT) r, (PLINT) g, (PLINT) b, (PLFLT) alpha);
  _palette.erase((PLINT)0);
  _palette.insert(std::make_pair((PLINT)0, rgba_t({r, g, b, alpha})));
  return true;
}

bool PLPLOT::Handle::col0a(int col0, int r, int g, int b, double alpha)
{
  _pls->scol0a((PLINT) col0, (PLINT) r, (PLINT) g, (PLINT) b, (PLFLT) alpha);
  _palette.erase((PLINT)col0);
  _palette.insert(std::make_pair((PLINT)col0, rgba_t({r, g, b, alpha})));
  return true;
}

bool PLPLOT::Handle::ptex(double x, double y, double dx, double dy, double just, const std::string& text)
{
  _pls->ptex((PLFLT) x, (PLFLT) y, (PLFLT) dx, (PLFLT) dy, (PLFLT) just, text.c_str());
  return true;
}

bool PLPLOT::Handle::mtex(const std::string& side, double disp, double pos, double just, const std::string& text)
{
  _pls->mtex(side.c_str(), (PLFLT) disp, (PLFLT) pos, (PLFLT) just, text.c_str());
  return true;
}

bool PLPLOT::Handle::box(const std::string& xopt, double xtick, int nxsub, const std::string& yopt, double ytick, int nysub)
{
  _pls->box(xopt.c_str(), (PLFLT) xtick, (PLINT) nxsub, yopt.c_str(), (PLFLT) ytick, (PLINT) nysub);
  return true;
}

bool PLPLOT::Handle::chr(double def, double scale)
{
  _pls->schr(def, scale);
  return true;
}

} /* namespace import */
} /* namespace bloc */
