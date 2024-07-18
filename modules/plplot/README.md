# Module PLplot for BLOC

The module "plplot" binds the PLplot API for BLOC. See the API reference from the PLplot main page.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "plplot".

```
import plplot;
```

Once loaded, you can print the object interface.
```
help plplot
```

Below, I expose few samples to create 2D and 3D plot.
Prior the simplest.
```
import plplot;

p = plplot("xwin");
p.init("-geometry 800x600");
p.env(0,100,0,100);

x = tab(100,num());
y = tab(100,num());
for i in 0 to 99 loop
  x.put(i, i);
  y.put(i, pow(num(i)/10.0, 2));
end loop;

p.line(x,y);
p.flush();
```
<p align="center">
  <img src="./sample.png" width="400" height="300"/>
<p>

Next a 3D mesh.
```
import plplot;

/* load tables of data */
XPTS = 35;
YPTS = 46;
X = tab(0, num());
Y = tab(0, num());
for I in 0 to (XPTS - 1) loop
    X.concat(((3.0 * (I - (XPTS / 2))) / (XPTS / 2)));
end loop;
for I in 0 to (YPTS - 1) loop
    Y.concat(((3.0 * (I - (YPTS / 2))) / (YPTS / 2)));
end loop;
Z = tab(0, tab(0, num()));
Z = tab(XPTS, tab(YPTS, num()));
for I in 0 to (XPTS - 1) loop
    XX = X.at(I);
    for J in 0 to (YPTS - 1) loop
        YY = Y.at(J);
        Z.at(I).put(J, (((((3.0 * (1.0 - XX)) * (1.0 - XX)) * exp((-(XX * XX) - ((YY + 1.0) * (YY + 1.0))))) - ((10.0 * (((XX / 5.0) - pow(XX, 3.0)) - pow(YY, 5.0))) * exp(((-XX * XX) - (YY * YY))))) - ((1.0 / 3.0) * exp(((-(XX + 1) * (XX + 1)) - (YY * YY))))));
    end loop;
end loop;

/* a function to initialize the colors map */
function cmap1_init() returns table is
begin
  /* initialize 4 tables of 2 decimals: Intensity, H, L, S */
  rt = tab(4, tab(2, num()));
  /* left and right boundaries */
  rt.at(0).put(0, 0.0);
  rt.at(0).put(1, 1.0);
  /* blue - green - yellow - red */
  rt.at(1).put(0, 240.0);
  rt.at(1).put(1, 0.0);
  /* L and S */
  rt.at(2).put(0, 0.6);
  rt.at(2).put(1, 0.6);
  rt.at(3).put(0, 0.8);
  rt.at(3).put(1, 0.8);
  return rt;
end;

P = plplot("xwin");
P.init("-geometry 800x600");

/* find the minimum and maximum of Z matrix */
zmax = null;
zmin = null;
P.minmax2dgrid(Z, XPTS, YPTS, zmax, zmin);

/* create color levels */
NLEVEL = 10;
step = ((zmax - zmin) / (NLEVEL + 1));
CLEVEL = tab(0, num());
for I in 0 to (NLEVEL - 1) loop
    CLEVEL.concat(((zmin + step) + (step * I)));
end loop;

/* set cmap1 colors */
cmap1 = cmap1_init();
P.scmap1n( 256 );
P.scmap1l( false, cmap1.at(0), cmap1.at(1), cmap1.at(2), cmap1.at(3) );

/* Draw ... */
P.adv(0);
P.col0(1);
P.vpor(0.0, 1.0, 0.0, 0.9);
P.wind(-1.0, 1.0, -1.0, 1.5);
P.w3d(1.0, 1.0, 1.2, -3.0, 3.0, -3.0, 3.0, zmin, zmax, 33.0, 24.0);
P.box3("bnstu", "x axis", 0.0, 0, "bnstu", "y axis", 0.0, 0, "bcdmnstuv", "z axis", 0.0, 4);
P.col0(2);

P.meshc(X, Y, Z, (1+2+4+8), CLEVEL);
P.flush();
P.pause(true);
```
<p align="center">
  <img src="./sample3d.png" width="400" height="300"/>
<p>

Finally a 2D log.
```
import plplot;

/* create table for data */
freq = tab(0,num()); /* frequencies */
ampl = tab(0,num()); /* amplitudes */
phas = tab(0,num()); /* phases */

/* load tables data for log plot */
L = 1.2e-3; /* 1.2mH */
C = 14e-6;  /* 14uF */
R = 8;      /* 8R */

F = 10.0; /* first band */
while F < 10e5 loop
  ZC = 1 / ( 2 * pi * F * C * ii );
  ZL = 2 * pi * F * L * ii;

  Z0 = R * ZC / ( R + ZC );
  Z = ZL + ( R * ZC / ( R + ZC ) );

  freq.concat( log10(F) );
  ampl.concat( 20 * log10(imag(Z0/Z)));
  phas.concat( 180 * iphase(Z0/Z) / pi );

  F = F * 1.1; /* loop next band */
end loop;

/* create the object PLplot, using the driver xwin */
P = plplot("xwin");
/* P = plplot("png", "sample2d.png"); */

/* set background color white */
P.scolbga(255,255,255,1.0);
/* customize colors palette */
P.scol0a(15,0,0,0,1.0);
P.scol0a(7,140,140,140,1.0);

/* initialize the plotting */
P.init("-geometry 800x600");
P.adv(0);
P.col0( 15 );
P.vpor( 0.15, 0.85, 0.1, 0.9 );
P.wind( log10(10), log10(20000), -24.0, +3.0 );

/* put x-axis */
P.col0( 15 );
P.box( "bcfghlnst", 0.0, 0, "bcghnstv", 0.0, 0 );
P.mtex( "b", 3.2, 0.5, 0.5, "frequency" );

/* draw amplitude */
P.col0( 1 );
P.width( 2.0 );
P.line( freq, ampl );
P.width( 0 );
/* put label for amplitude */
p.mtex( "l", 5.0, 0.5, 0.5, "amplitude (dB)" );

/* draw phase */
P.col0( 9 );
P.schr(0, 0.5);
P.wind( log10(10), log10(20000), -180.0, 180.0 );
P.box( "", 0.0, 0, "cmstv", 30.0, 3 );
P.schr(0, 1.0);
P.width( 2.0 );
P.line( freq, phas );
P.width( 0 );
/* put label for phase */
P.mtex( "r", 5.0, 0.5, 0.5, "phase shift (degrees)" );

/* Put title */
P.col0( 15 );
P.width( 2.0 );
P.mtex( "t", 2.0, 0.5, 0.5, "Low-Pass Filter" );
P.width( 0 );

P.flush();
P.pause(true);
```
<p align="center">
  <img src="./sample2d.png" width="400" height="300"/>
<p>

