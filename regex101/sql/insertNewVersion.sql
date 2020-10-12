INSERT INTO versions
            (regex,
             teststring,
             flags,
             delimeter,
             flavor,
             version,
             expressionId,
             substString)
VALUES      (:regex,
             :testString,
             :flags,
             :delimeter,
             :flavor,
              (SELECT Max(versions.version)+1 FROM versions WHERE versions.expressionId=(SELECT id FROM expressions WHERE permalinkFragment=:permalinkFragment)),
              (SELECT id FROM expressions WHERE permalinkFragment=:permalinkFragment),
             :substitution)


