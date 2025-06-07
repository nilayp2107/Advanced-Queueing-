 -- aqvectorcreate.sql 
-- Description - Create Classic Vector Queue in Database

-- To run this .sql file 
-- Step 1. run the sqlplus CLI
-- Step 2. SQL> @aqrawcreate

-- Create Vector Classic Queue
BEGIN
   DBMS_AQADM.CREATE_QUEUE_TABLE(
      queue_table            => 'vector_queue_table',
      queue_payload_type     => 'VECTOR');
END;
/
-- Create RAW queue under the same VECTOR Queue Table
BEGIN
   DBMS_AQADM.CREATE_QUEUE(
      queue_name          => 'vector_queue',
      queue_table         => 'vector_queue_table');
END;
/
-- Start the queue
BEGIN
 DBMS_AQADM.START_QUEUE(
    queue_name => 'vector_queue');
END;
/
