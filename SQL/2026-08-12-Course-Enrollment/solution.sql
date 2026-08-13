SELECT
  c.title AS course_title,
  COUNT(e.id) AS enrollment_count
FROM courses c
LEFT JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC, course_title ASC;