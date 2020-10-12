INSERT INTO favorites (userId, expressionId) VALUES (:userId, (SELECT id FROM expressions WHERE permalinkFragment=:permalinkFragment));




