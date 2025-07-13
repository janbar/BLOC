import file;

function HASHSTR(key:string) return string is
begin
  return hex(hash(key), 8);
end;

function generate_source(lang:string, path:string) return string is
begin
  print "... Processing lang [" lang "]";
  out = file("msgdb_lang_" + lang + ".h","w");
  out.write("#include ""msgdb_common.h""\n\n");

  # section: name, path
  section = tab(1, tup("main", path));
  prefix = "msgdb_" + lang;

  forall e in file().dir(path) loop
    if e@1 == 2 and isnull(strpos(e@3, ".")) then
      section.concat(tup(e@3, path + file().separator() + e@3));
    end if;
  end loop;

  forall s in section loop
    secsrc = tab(0, str());
    forall k in file().dir(s@2) loop
      if k@1 == 1 and k@3 matches "msg_.*\\.txt" then
        print "... Processing file " s@2 file().separator() k@3;
        # Extract the keyword from filename
        keyword = replace(substr(k@3, 4, k@3.count()-8), "_", " ");
        txt = file(s@2 + file().separator() + k@3, "r");
        buf:bytes, tmp:bytes, while txt.read(tmp, 1024) > 0 loop buf.concat(tmp); end loop;
        txt.close();
        txtname = prefix + "_" + HASHSTR(s@1 + ":" + keyword) + "_txt";

        # fill section source
        secsrc.concat("{ """ + keyword + """, " + str(buf.count()) + ", " + txtname + " },\n");

        out.write("static const unsigned char " + txtname + "[] = {\n");
        for i in 1 to buf.count() loop
          out.write("0x" + hex(buf.at(i-1), 2));
          if i < buf.count() then
            if i % 12 == 0 then out.write(",\n  "); else out.write(", "); end if;
          end if;
        end loop;
        out.write("};\n\n");
      end if;
    end loop;

    # write section source
    print "... Processing section " lang "/" s@1;
    out.write("static const MSGDB_KEYWORD_ROW " + prefix + "_kw_" + s@1 + "[] = {\n");
    forall src in secsrc loop
      out.write("  " + src);
    end loop;
    out.write("  { 0x0, 0x0, 0x0 },\n");
    out.write("};\n\n");
  end loop;

  out.write("static const MSGDB_SECTION_ROW " + prefix + "_section[] = {\n");
  forall s in section loop
    out.write("  { """ + s@1 + """, " + prefix + "_kw_" + s@1 + " },\n");
  end loop;
  out.write("  { 0x0, 0x0 },\n");
  out.write("};\n\n");

  r = out.filename();
  print "... File " r " has been updated";
  out.close();
  return r;
end;

# Rewrite the main header
main = file("msgdb_lang.h", "w");
main.write("#ifndef MSGDB_LANG_H\n");
main.write("#define MSGDB_LANG_H\n");
main.write("\n");

langsrc = tab(0, str());

# for all languages, generate an header
forall e in file().dir(".") loop
  if e@1 == 2 and e@3 matches "lang_.*" then
    lang = substr(e@3, 5);
    fn = generate_source(lang, e@3);
    main.write("#include """ + fn + """\n");
    langsrc.concat("{ """ + lang + """, " + "msgdb_" + lang + "_section },\n");
  end if;
end loop;
main.write("\n");

main.write("static const MSGDB_LANG_ROW msgdb_lang[] = {\n");
forall src in langsrc loop
   main.write("  " + src);
end loop;
main.write("  { 0x0, 0x0 },\n");
main.write("};\n");
main.write("\n#endif /* MSGDB_LANG_H */\n");

print "... File " main.filename() " has been updated";
main.close();

print "Elapsed: " getsys("clock");
