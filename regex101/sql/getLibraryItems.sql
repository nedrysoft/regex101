SELECT *
FROM   expressions
       INNER JOIN versions
               ON versions.expressionId = expressions.id
                  AND versions.version = (SELECT Max(versions.version)
                                          FROM   versions
                                          WHERE  versions.expressionId =
                                                 expressions.id)
                  AND (expressions.isLibraryEntry IS TRUE)
                  AND ( ( expressions.title LIKE :search)
                         OR ( expressions.description LIKE :search)
                         OR ( expressions.title IS NULL) )
ORDER  BY :orderBy;
