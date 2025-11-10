/**
 * Clean text for the reference manual
 * It removes all unwanted UTF-8 artifacts.
 */
import file;
import utf8;

// Open the document for read 
SRC = file("BLOC-Reference-Manual.md","r");

// Load the content 
data:string;
buf:string;
while SRC.readln(buf) loop
  data.concat(buf);
end loop;

// Fix content using UTF8 transliterate
FIXED = utf8(data).translit().string();

// A crude test
if FIXED == data then
  print ">>> " SRC.filename() " is clean.";
else
  // Save result in new copy
  OUT = file("BLOC-Reference-Manual.md.CLEANED","w");
  OUT.write(FIXED);
  print ">>> cleanup done: " OUT.filename();
  OUT.close();
end if;

SRC.close();

