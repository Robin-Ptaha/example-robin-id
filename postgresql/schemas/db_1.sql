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
