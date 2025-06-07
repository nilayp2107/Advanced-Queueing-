 -- teqrawcreate.sql 
-- Description - Create Raw Queue in Database

-- To run this .sql file 
-- Step 1. run the sqlplus CLI
-- Step 2. SQL> @teqrawcreate

-- Create RAW Transactional Event Queue
BEGIN
   DBMS_AQADM.CREATE_TRANSACTIONAL_EVENT_QUEUE(
      queue_name          => 'vector_queue_teq',
      queue_payload_type     => 'VECTOR');
END;
/
-- Start the queue
BEGIN
 DBMS_AQADM.START_QUEUE(
    queue_name => 'vector_queue_teq');
END;
/