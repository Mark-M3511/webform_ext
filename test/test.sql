SELECT r . * , COALESCE( rb.blocked_days, 0 ) AS  'num_blocked_days', COALESCE(rb.available_date, CURDATE()) as 'next_available_date'
FROM zen_vupoint_rooms r
LEFT OUTER JOIN zen_vupoint_room_bookings rb ON ( r.rid = rb.rid ) 
where COALESCE(rb.available_date, '2013-06-10') >= ADDDATE('2013-06-10', -60)
ORDER BY rb.blocked_days, rb.available_date, r.rid
LIMIT 0 , 30

SELECT r. * , COALESCE( rb.blocked_days, 0 ) AS  'num_blocked_days', COALESCE( rb.available_date, CURDATE( ) ) AS  'next_available_date'
FROM zen_vupoint_rooms r
LEFT OUTER JOIN zen_vupoint_room_bookings rb ON ( r.rid = rb.rid ) 
WHERE COALESCE( rb.available_date,  '2013-06-10' ) >= ADDDATE(  '2013-06-10', -60 ) 
ORDER BY rb.available_date, r.rid
LIMIT 0 , 30

SELECT r. * , COALESCE( rb.blocked_days, 0 ) AS  'num_blocked_days', COALESCE( rb.available_date, CURDATE( ) ) AS  'next_available_date'
FROM zen_vupoint_rooms r
LEFT OUTER JOIN zen_vupoint_room_bookings rb ON ( r.rid = rb.rid ) 
WHERE (COALESCE( rb.available_date,  '2013-07-16' ) BETWEEN ADDDATE(  '2013-07-16', -60 ) AND ADDDATE(  '2013-07-16', 60 )) AND (r.available = 1) AND (r.private_amenities = 1)
ORDER BY rb.blocked_days, rb.available_date, r.rid
LIMIT 0 , 100

------------------------------------------------------------------------------------
SELECT field_data_field_arrival_date.field_arrival_date_value AS field_data_field_arrival_date_field_arrival_date_value, node.nid AS nid_1, COUNT( node.nid ) AS nid
FROM node node
LEFT JOIN field_data_field_arrival_date field_data_field_arrival_date ON node.nid = field_data_field_arrival_date.entity_id
AND (
field_data_field_arrival_date.entity_type =  'node'
AND field_data_field_arrival_date.deleted =  '0'
)
WHERE (
(
(
node.status =  '1'
)
AND (
DATE_FORMAT( field_data_field_arrival_date.field_arrival_date_value2,  '%Y-%m' ) >=  '2013-05'
AND DATE_FORMAT( field_data_field_arrival_date.field_arrival_date_value,  '%Y-%m' ) <=  '2013-05'
)
)
AND (
(
(
node.type
IN (
'booking'
)
)
)
)
)
GROUP BY field_data_field_arrival_date_field_arrival_date_value, nid_1
ORDER BY field_data_field_arrival_date_field_arrival_date_value ASC 
LIMIT 0 , 30
----------------------------------------------------------------------------------------------
SELECT field_data_field_arrival_date.field_arrival_date_value AS field_data_field_arrival_date_field_arrival_date_value, COUNT( node.nid ) AS nid
FROM node node
LEFT JOIN field_data_field_arrival_date field_data_field_arrival_date ON node.nid = field_data_field_arrival_date.entity_id
AND (
field_data_field_arrival_date.entity_type =  'node'
AND field_data_field_arrival_date.deleted =  '0'
)
WHERE (
(
(
node.status =  '1'
)
AND (
DATE_FORMAT( field_data_field_arrival_date.field_arrival_date_value2,  '%Y-%m' ) >=  '2013-05'
AND DATE_FORMAT( field_data_field_arrival_date.field_arrival_date_value,  '%Y-%m' ) <=  '2013-05'
)
)
AND (
(
(
node.type
IN (
'booking'
)
)
)
)
)
GROUP BY field_data_field_arrival_date_field_arrival_date_value
ORDER BY field_data_field_arrival_date_field_arrival_date_value ASC 
LIMIT 0 , 30
--------------------------------------------------------------------------------
SELECT field_data_field_arrival_date.field_arrival_date_value, COUNT( node.nid ) AS nid
FROM node node
LEFT JOIN field_data_field_arrival_date field_data_field_arrival_date ON node.nid = field_data_field_arrival_date.entity_id
AND (
field_data_field_arrival_date.entity_type =  'node'
AND field_data_field_arrival_date.deleted =  '0'
)
WHERE (
(
(
node.status =  '1'
)
AND (
DATE_FORMAT( field_data_field_arrival_date.field_arrival_date_value,  '%Y-%m' ) >=  '2013-05'
AND DATE_FORMAT( field_data_field_arrival_date.field_arrival_date_value,  '%Y-%m' ) <=  '2013-05'
)
)
AND (
(
(
node.type
IN (
'booking'
)
)
)
)
)
GROUP BY field_data_field_arrival_date.field_arrival_date_value
LIMIT 0 , 30
=====================================================================
SELECT  `bd`.`booked_date` , COALESCE(  `bd2`.`blocked` , 0 ) AS  `is_blocked` , COUNT( * ) AS  `date_count` 
FROM  `zen_vupoint_booked_dates`  `bd` 
LEFT JOIN  `zen_vupoint_blocked_dates`  `bd2` ON (  `bd`.`booked_date` =  `bd2`.`blocked_date` ) 
GROUP BY  `bd`.`booked_date` ,  `is_blocked` 
HAVING  `date_count` >=10
LIMIT 0 , 30
========================================================================
SELECT r.*, rb.available_date AS available_date, rb.blocked_days AS blocked_days, COALESCE(rb.available_date, '2013-07-07') AS next_available_date, COALESCE(rb.blocked_days, 0) AS num_blocked_days FROM zen_vupoint_rooms r LEFT OUTER JOIN zen_vupoint_room_bookings rb ON r.rid = rb.rid WHERE ((COALESCE(rb.available_date, '2013-07-07') BETWEEN '2013-05-07' AND '2013-09-04')) AND (r.available = 1) AND (r.private_amenities IN (0)) ORDER BY rb.available_date ASC, r.rid ASC LIMIT 100 OFFSET 0
=========================================================================
SELECT COUNT( * ) as count_rooms
FROM  `zen_vupoint_booked_dates` zbd
INNER JOIN `zen_vupoint_rooms` r ON (r.rid = zbd.rid)
WHERE r.code = 'PB'
HAVING count_rooms > 6
=========================================================================
SELECT @room_count := COUNT( * ) 
FROM zen_vupoint_rooms
LIMIT 0 , 30

=========================================================================
SET @count_of_rooms := 9;

SELECT booked_date, COUNT( confirmed ) 
FROM zen_vupoint_booked_dates
GROUP BY booked_date
having COUNT( confirmed ) >= @count_of_rooms;
having COUNT( confirmed ) >= @count_of_rooms;