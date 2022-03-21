# min_to_max

This Extension is Used to create an aggregate that process a column and returns the min and max values in a format of min->max.

It can handle smallint, integer, bigint, real or double precision.

Steps to install:

Clone the repo

Change the Directory to repo

make

make install

CREATE EXTENSION min_to_max;

SELECT min_to_max(column_name) FROM table_name.

Thanks.
