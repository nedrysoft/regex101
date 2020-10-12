SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.permalinkfragment = expressions.permalinkfragment
                  AND versions.version = (SELECT Max(versions.version)
                                          FROM   versions
                                          WHERE  versions.permalinkfragment =
                                                 expressions.permalinkfragment)
                  AND ( ( expressions.title LIKE :search)
                         OR ( expressions.description LIKE :search) )
ORDER  BY :orderBy;
