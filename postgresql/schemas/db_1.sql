create extension if not exists citext;
create extension if not exists pgcrypto;


drop schema if exists robinid cascade;
create schema if not exists robinid;


create table if not exists robinid.users (
    id uuid primary key default gen_random_uuid(),
    username citext unique not null,
    email varchar(255) not null,
    password_hash varchar(255) not null
);


create type robinid.user as (
    id text,
    username citext,
    email varchar(255),
    password_hash varchar(255)
);


create or replace function robinid.add_new_user(
    _username citext,
    _email varchar(255),
    _password_hash varchar(255)
)
returns text
as $$
declare
    _user_id text;
begin
    insert into
        robinid.users(username, email, password_hash)
    values
        (_username, _email, _password_hash)
    returning
        id
    into
        _user_id;

    return _user_id::text;
end;
$$ language plpgsql;


create or replace function robinid.get_user_by_username(
    _username citext
)
returns setof robinid.user
as $$
begin
    return query
    select
        id::text,
        username,
        email,
        password_hash
    from
        robinid.users
    where
        username = _username;
end;
$$ language plpgsql;
