SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.permalinkfragment = expressions.permalinkfragment
WHERE  expressions.permalinkfragment = :permalinkFragment
       AND versions.version = (SELECT Max(versions.version)
                               FROM   versions
                               WHERE  versions.permalinkfragment =
                                      expressions.permalinkfragment)
