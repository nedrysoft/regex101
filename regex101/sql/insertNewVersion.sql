INSERT INTO versions
            (regex,
             teststring,
             flags,
             delimeter,
             flavor,
             version,
             permalinkfragment)
VALUES      (:regex,
             :testString,
             :flags,
             :delimeter,
             :flavor,
              (SELECT Max(versions.version)+1 FROM versions WHERE versions.permalinkFragment=:permalinkFragment),
             :permalinkFragment)

