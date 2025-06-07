 -- aqrawcreate.sql 
-- Description - Create Raw Queue in Database

-- To run this .sql file 
-- Step 1. run the sqlplus CLI
-- Step 2. SQL> @aqrawcreate

-- Create RAW Classic Queue
BEGIN
   DBMS_AQADM.CREATE_QUEUE_TABLE(
      queue_table            => 'raw_queue_table',
      queue_payload_type     => 'RAW');
END;
/
-- Create RAW queue under the same RAW Queue Table
BEGIN
   DBMS_AQADM.CREATE_QUEUE(
      queue_name          => 'raw_queue',
      queue_table         => 'raw_queue_table');
END;
/
-- Start the queue
BEGIN
 DBMS_AQADM.START_QUEUE(
    queue_name => 'raw_queue');
END;
/
