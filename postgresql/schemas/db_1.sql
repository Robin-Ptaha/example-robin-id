CREATE EXTENSION IF NOT EXISTS citext;
CREATE EXTENSION IF NOT EXISTS pgcrypto;

DROP SCHEMA IF EXISTS robinid CASCADE;
CREATE SCHEMA IF NOT EXISTS robinid;

CREATE TABLE IF NOT EXISTS robinid.users (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    username CITEXT UNIQUE NOT NULL,
    email VARCHAR(255) NOT NULL,
    password_hash VARCHAR(255) NOT NULL
);


CREATE TYPE robinid.user AS (
    id TEXT,
    username CITEXT,
    email VARCHAR(255),
    password_hash VARCHAR(255)
);


CREATE OR REPLACE FUNCTION robinid.add_new_user(
    _username CITEXT,
    _email VARCHAR(255),
    _password_hash VARCHAR(255)
)
RETURNS TEXT
AS $$
DECLARE
    _user_id TEXT;
BEGIN
    INSERT INTO robinid.users (username, email, password_hash)
    VALUES (_username, _email, _password_hash)
    RETURNING id INTO _user_id;

    RETURN _user_id::TEXT;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION robinid.get_user_by_username(
    _username CITEXT
)
RETURNS SETOF robinid.user
AS $$
BEGIN
    RETURN QUERY
    SELECT
        id::TEXT,
        username,
        email,
        password_hash
    FROM
        robinid.users
    WHERE
        username = _username;
END;
$$ LANGUAGE plpgsql;
