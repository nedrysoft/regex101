

CREATE TABLE expressions (
  id integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  title varchar(128),
  description varchar(128),
  dateModified varchar(128),
  author varchar(128),
  permalinkFragment varchar(128),
  upvotes integer(128),
  downvotes integer(128),
  userVote integer(128),
  error varchar(128)
);


CREATE TABLE tags (
  id integer PRIMARY KEY NOT NULL,
  permalinkFragment varchar(128),
  tag varchar(128)
);


CREATE TABLE users (
  userId integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  email varchar(128)
);


CREATE TABLE versions (
  id integer PRIMARY KEY NOT NULL,
  version integer(128),
  permalinkFragment varchar(128),
  regex varchar(128),
  testString varchar(128),
  substString varchar(128),
  flags varchar(128),
  delimeter varchar(128),
  flavor varchar(128)
);

INSERT INTO users
            (email)
VALUES     ("default");

