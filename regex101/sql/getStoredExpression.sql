SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.expressionId = expressions.id
       INNER JOIN (SELECT COUNT(*) AS versionCount FROM versions)
              ON versions.expressionId = expressions.id
WHERE  expressions.permalinkfragment=:permalinkFragment
       AND versions.version = :version;
