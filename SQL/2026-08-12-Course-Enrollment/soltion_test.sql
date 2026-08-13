CREATE TABLE courses (
  id INTEGER PRIMARY KEY,
  title TEXT NOT NULL,
  difficulty TEXT NOT NULL
);

CREATE TABLE enrollments (
  id INTEGER PRIMARY KEY,
  course_id INTEGER NOT NULL,
  student_name TEXT NOT NULL
);

INSERT INTO courses (id, title, difficulty)
VALUES (1, 'Intro to Databases', 'beginner');

INSERT INTO courses (id, title, difficulty)
VALUES (2, 'SQL for Data Analysis', 'intermediate');

INSERT INTO courses (id, title, difficulty)
VALUES (3, 'Advanced Indexing Strategies', 'advanced');

INSERT INTO courses (id, title, difficulty)
VALUES (4, 'Normalization Deep Dive', 'advanced');

INSERT INTO courses (id, title, difficulty)
VALUES (5, 'ACID Transactions Explained', 'intermediate');

INSERT INTO courses (id, title, difficulty)
VALUES (6, 'Distributed Systems and Sharding', 'advanced');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (1, 1, 'Alice');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (2, 1, 'Bob');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (3, 2, 'Charlie');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (4, 2, 'Dana');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (5, 2, 'Evan');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (6, 3, 'Fiona');

INSERT INTO enrollments (id, course_id, student_name)
VALUES (7, 4, 'George');
