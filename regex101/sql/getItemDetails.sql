SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.expressionId = expressions.id
WHERE  expressions.permalinkfragment = :permalinkFragment
       AND versions.version = (SELECT Max(versions.version)
                               FROM   versions
                               WHERE  versions.expressionId =
                                      expressions.id);
