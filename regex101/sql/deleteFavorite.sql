DELETE FROM favorites
WHERE  expressionId=(SELECT id FROM expressions WHERE permalinkFragment=:permalinkFragment);
AND     userId=:userId
