 -- aqvectorenq.sql 
-- Description - Enqueue a Vector Message in Classic Queue

-- To run this .sql file 
-- Step 1. run the sqlplus CLI
-- Step 2. SQL> @aqvectorenq

-- Enqueue A message [34.6, 77.8, 100, 45.67891230]

DECLARE
    msgid RAW(16);
    enq_options DBMS_AQ.ENQUEUE_OPTIONS_T;
    msg_properties DBMS_AQ.MESSAGE_PROPERTIES_T;
    vector_message VECTOR;
BEGIN
    vector_message := TO_VECTOR('[34.6, 77.8, 100, 45.67891230]');
    DBMS_AQ.ENQUEUE(
        queue_name          =>  'vector_queue',
        enqueue_options     =>  enq_options,
        message_properties  =>  msg_properties,
        payload             =>  vector_message,
        msgid               =>  msgid
    );
    COMMIT;
END;
/