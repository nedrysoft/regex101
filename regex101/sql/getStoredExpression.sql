SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.permalinkfragment = expressions.permalinkfragment
WHERE  expressions.permalinkfragment = :permalinkFragment
       AND versions.version = :version;
