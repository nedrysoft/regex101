SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.permalinkfragment = expressions.permalinkfragment
       INNER JOIN (SELECT COUNT(*) AS versionCount FROM versions)
              ON versions.permalinkfragment = expressions.permalinkfragment
WHERE  expressions.permalinkfragment='XYNZFJ'
       AND versions.version = :version;
