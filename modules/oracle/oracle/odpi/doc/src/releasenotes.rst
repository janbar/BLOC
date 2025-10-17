ODPI-C Release notes
====================

Version 5.6.4 (TBD)
-------------------

#)  Fixed bug that caused a segfault to take place when fetching LOBs without
    enabling threaded mode
    (`issue 200 <https://github.com/oracle/odpi/issues/200>`__).
#)  Fixed bug that prevented an error from being returned when attempting to
    scroll before the start of the result set with mode
    ``DPI_MODE_FETCH_PRIOR``
    (`issue 201 <https://github.com/oracle/odpi/issues/201>`__).
#)  Fixed bug that caused ``ORA-03137: malformed TTC packet from client
    rejected`` exception to be raised when attempting to execute a scrollable
    statement with mode ``DPI_MODE_EXEC_DESCRIBE_ONLY``
    (`issue 202 <https://github.com/oracle/odpi/issues/202>`__).
#)  Fixed segfault on some platforms when trying to access vector flags on
    Oracle type information.
#)  Connectivity and interoperability with Oracle Database and Oracle Client
    libraries older than version 19 is deprecated and will be removed in a
    future version of ODPI-C. Production use, and availability of database and
    client software, is detailed in `Release Schedule of Current Database
    Releases <https://support.oracle.com/epmos/faces/
    DocumentDisplay?id=742060.1>`__.


Version 5.6.3 (October 6, 2025)
-------------------------------

#)  Add NULL terminator to buffers used for returning NUMBER column data as
    text. This eliminates the need for making a copy when using C conversion
    functions such as ``strtoll()`` and ``strtod()``.
#)  Fixed bug with error wrapping which could result in garbage characters
    being introduced. Fixed potential bug when truncation could occur with very
    large error messages.


Version 5.6.2 (July 29, 2025)
-----------------------------

#)  Fixed bug resulting in a segfault when environment variable
    ``DPI_DEBUG_LEVEL`` includes flag ``DPI_DEBUG_LEVEL_LOAD_LIB`` on Windows
    (`issue 197 <https://github.com/oracle/odpi/issues/197>`__).
#)  Fixed bug that masked the error thrown when a PL/SQL fixup callback for
    session state failed.
#)  Fixed null checks and version checks for functions
    :func:`dpiConn_beginSessionlessTransaction()`,
    :func:`dpiConn_resumeSessionlessTransaction()`, and
    :func:`dpiConn_suspendSessionlessTransaction()`.


Version 5.6.1 (July 16, 2025)
-----------------------------

#)  Fixed bug resulting in a segfault when a pooled connection is returned to
    the pool with an active sessionless transaction.


Version 5.6.0 (June 26, 2025)
-----------------------------

#)  Added support for Oracle Database 23ai sessionless transactions with new
    APIs - :func:`dpiConn_beginSessionlessTransaction()`,
    :func:`dpiConn_resumeSessionlessTransaction()` and
    :func:`dpiConn_suspendSessionlessTransaction()`. Also, added a new datatype
    :ref:`dpiSessionlessTransactionId<dpiSessionlessTransactionId>`.
#)  Added support for fetching SQL_ID
    (`issue 194 <https://github.com/oracle/odpi/issues/194>`__).
#)  Fixed bug resulting in a segfault when fetching nested collections.
#)  Fixed bug resulting in a non-zero integer value for the registration ID of
    Oracle Advanced Queueing subscriptions.
#)  Adjusted Makfile to allow for greater flexbility in building, as suggested
    (`issue 196 <https://github.com/oracle/odpi/issues/196>`__).


Version 5.5.1 (March 25, 2025)
------------------------------

#)  Fixed bug resulting in a segfault when unable to load the Oracle Client
    libraries.
#)  Fixed bug which resulted in error ``ORA-24328: illegal attribute value``
    when calling :func:`dpiConn_getObjectType()` with Oracle Client libraries
    11.2.
#)  Improved error message when calling :func:`dpiVector_getValue()` and
    :func:`dpiVector_setValue()` with sparse vectors and Oracle Client
    libraries 23.6 or earlier.
#)  Improved error message when calling :func:`dpiConn_getMaxOpenCursors()`
    when using Oracle Client libraries 11.2.


Version 5.5.0 (February 25, 2025)
---------------------------------

#)  Added support for Oracle Database 23.7 SPARSE vectors.
#)  Added support for giving the path of the relative Oracle Client
    configuration directory if no configuration directory was supplied when
    calling :func:`dpiContext_createWithParams()`.
#)  Fixed bug that cleared the isRecoverable flag when an Oracle error is
    wrapped by an ODPI-C specific error.
#)  Added method :func:`dpiData_getVector()` to retrieve the value of the data
    when the native type is `DPI_NATIVE_TYPE_VECTOR`.


Version 5.4.1 (December 11, 2024)
---------------------------------

#)  Fixed bug calculating the maximum identifier length when using Oracle
    Client libraries 12.1, or older. The returned value may now be 0 when the
    size cannot be reliably determined by ODPI-C, which occurs when using
    Oracle Client libraries 12.1 (or older) to connect to Oracle Database 12.2,
    or later.
#)  Fixed bug resulting in a segfault when using external authentication
    (`python-oracledb issue 425
    <https://github.com/oracle/python-oracledb/issues/425>`__).


Version 5.4.0 (November 5, 2024)
--------------------------------

#)  Added support for getting information about a connection in the structure
    :ref:`dpiConnInfo<dpiConnInfo>` using the new method
    :func:`dpiConn_getInfo()`.
#)  Fixed bug resulting in error ``ORA-25707: The token is invalid`` when
    using IAM token based authentication and creating multiple pooled
    connections.
#)  Fixed bug affecting Application Continuity with older Oracle Client
    libraries by ensuring that the mode indicating bound REF CURSOR statement
    handles are never re-used is only enabled with Oracle Client libraries 23.6
    and higher.
#)  Disable support added in Oracle Client 23ai for connection pool ping
    intervals since this functionality is already available in ODPI-C.


Version 5.3 (July 23, 2024)
---------------------------

#)  Added support for vector storage format ``DPI_VECTOR_FORMAT_BINARY``
    available in Oracle Database 23.5 and higher.
#)  Eliminated memory leak when dequeing messages with JSON payloads
    (`python-oracledb issue 346
    <https://github.com/oracle/python-oracledb/issues/346>`__).
#)  Internal change: let the Oracle Client library know that bound REF CURSOR
    statement handles are never re-used.


Version 5.2 (March 11, 2024)
----------------------------

#)  Added methods :func:`dpiSodaDb_createJsonDocument()`,
    :func:`dpiSodaDoc_getIsJson()` and :func:`dpiSodaDoc_getJsonContent()` in
    order to support using JSON when fetching and binding SODA documents with
    Oracle Client 23 onward. This allows for seamless transfer of extended data
    types. In order to make use of this the member
    :member:`dpiContextCreateParams.sodaUseJsonDesc` must be set to 1 before
    calling :func:`dpiContext_createWithParams()`.
#)  Added new type ``DPI_ORACLE_TYPE_JSON_ID`` for JSON ids. In order to make
    use of this the member :member:`dpiContextCreateParams.useJsonId` must be
    set to 1 before calling :func:`dpiContext_createWithParams()`; otherwise,
    these values are returned as type ``DPI_ORACLE_TYPE_RAW``.
#)  Added attribute :member:`dpiDataTypeInfo.isOson` which is set to true when
    columns are fetched that have the "IS JSON FORMAT OSON" constraint enabled.
#)  Error ``DPI-1010: not connected`` is now raised when attempting to use a
    LOB with a closed connection instead of
    ``DPI-1040: LOB was already closed``.
#)  Errors ``DPI-1067: call timeout of %u ms exceeded with ORA-%05d`` and
    ``DPI-1080: connection was closed by ORA-%05d`` now retain the original
    error message.
#)  Added support for vectors. This requires Oracle Database 23ai and Oracle
    Client 23.


Version 5.1 (December 19, 2023)
-------------------------------

#)  Added members :member:`dpiDataTypeInfo.domainSchema`,
    :member:`dpiDataTypeInfo.domainName`, and
    :member:`dpiDataTypeInfo.annotations` which provide the
    `SQL domain <https://docs.oracle.com/en/database/oracle/oracle-database/23/sqlrf/create-domain.html#GUID-17D3A9C6-D993-4E94-BF6B-CACA56581F41>`__
    and
    `annotations <https://docs.oracle.com/en/database/oracle/oracle-database/23/sqlrf/annotations_clause.html#GUID-1AC16117-BBB6-4435-8794-2B99F8F68052>`__
    associated with a column in Oracle Database 23ai.  Also requires Oracle
    Client 23.1 or higher.
#)  Added methods :func:`dpiConn_getDbDomain()`, :func:`dpiConn_getDbName()`,
    :func:`dpiConn_getMaxOpenCursors()`, :func:`dpiConn_getServiceName()`,
    and :func:`dpiConn_getTransactionInProgress()` in order to provide
    additional metadata about the database.
#)  Added new type ``DPI_ORACLE_TYPE_XMLTYPE`` for data of type ``SYS.XMLTYPE``
    to the :ref:`dpiOracleTypeNum<dpiOracleTypeNum>` enumeration. Previously
    data of this type was returned as ``DPI_ORACLE_TYPE_LONG_VARCHAR``,
    which was unclear.


Version 5.0.1 (October 10, 2023)
--------------------------------

#)  Fixed bug resulting in a segfault on some platforms when using two-phase
    commit.


Version 5.0 (August 15, 2023)
-----------------------------

#)  Added function :func:`dpiSodaColl_listIndexes()` in order to get a list of
    the indexes for a SODA collection.
#)  Added function :func:`dpiContext_freeStringList()` which replaces the
    function dpiSodaDb_freeCollectionNames(). The new structure
    :ref:`dpiStringList<dpiStringList>` replaces the old structure
    dpiSodaCollNames. The old names are deprecated and will be removed in a
    future version.
#)  Added support for member :member:`dpiSodaOperOptions.lock`.
    This can only be used with Oracle Client 21.3 and higher (also available in
    Oracle Client 19 from 19.11).
#)  Added attribute :member:`dpiDataTypeInfo.isJson` which is set to true when
    columns are fetched that are of type `DPI_ORACLE_TYPE_JSON` or have the
    "IS JSON" constraint enabled.
#)  Added method :func:`dpiConn_getInstanceName()` in order to be able to get
    the Oracle Database instance name associated with the connection. This is
    the same value as given by the SQL expression
    ``sys_context('userenv', 'instance_name')``.
#)  Added support for the embedded OIDs found in SODA documents in Oracle
    Database 23ai.
#)  Adjusted the value of the attribute :member:`dpiJsonNode.nativeTypeNum` to
    be `DPI_NATIVE_TYPE_FLOAT` when the value stored in JSON is a float value.
    Previously the value was `DPI_NATIVE_TYPE_DOUBLE`
    (`issue 174 <https://github.com/oracle/odpi/issues/174>`__).
#)  Added support for automatically retrying a query if the error
    ``ORA-00932: inconsistent data types`` is raised (which can occur if a
    table or view is recreated with a data type that is incompatible with
    the column's previous data type).
#)  Dropped functions dpiConn_beginDistribTrans() and
    dpiConn_prepareDistribTrans() which were deprecated in version 4.3.


Version 4.6.1 (March 30, 2023)
------------------------------

#)  Added support for the "signed int", "signed long" and "decimal128" scalar
    types in JSON (generally only seen when converting from MongoDB).


Version 4.6 (November 10, 2022)
-------------------------------

#)  Added attributes :member:`~dpiObjectTypeInfo.packageName` and
    :member:`~dpiObjectTypeInfo.packageNameLength` to structure
    :ref:`dpiObjectTypeInfo<dpiObjectTypeInfo>`.
#)  Defer debugging startup until first use of an ODPI-C function instead of
    upon loading of the library.


Version 4.5 (September 13, 2022)
--------------------------------

#)  Added support for OAuth token authentication when creating standalone
    connections and connection pools.
#)  Added support for enqueuing and dequeuing JSON payloads using Advanced
    Queuing (AQ). Functions :func:`dpiConn_newJson()`,
    :func:`dpiConn_newJsonQueue()`, :func:`dpiMsgProps_getPayloadJson()` and
    :func:`dpiMsgProps_setPayloadJson()` were added to support this
    functionality.


Version 4.4.1 (June 14, 2022)
-----------------------------

#)  Fixed ability to use constant `DPI_ORACLE_TYPE_LONG_NVARCHAR` when creating
    variables to fetch NCLOB values.
#)  Adjusted test suite and samples for token based authentication to more
    closely follow the behavior of the Oracle Client libraries.


Version 4.4 (May 20, 2022)
--------------------------

#)  Added support for token based authentication when establishing pool based
    connections and standalone connections.
#)  Added method :func:`dpiMsgProps_setRecipients()` to support the recipients
    list message property of AQ.
#)  Added attribute :member:`dpiSubscrMessage.aqMsgId` which provides the
    message id of an AQ message that is available to dequeue.
#)  Added constant `DPI_ORACLE_TYPE_LONG_NVARCHAR`. This type cannot be
    represented in an Oracle database but is used when fetching NCLOB data as a
    string.
#)  Fixed bug resulting in a segfault when certain errors occur during the
    binding of a variable
    (`issue 168 <https://github.com/oracle/odpi/issues/168>`__).
#)  Prefer the use of memcpy() over strncpy(), even though embedded NULL
    characters in XID values are extremely unlikely
    (`issue 175 <https://github.com/oracle/odpi/issues/175>`__).
#)  Fixed bug resulting in error ORA-25263 when attempting to dequeue a message
    with a specific message ID that is known to exist.
#)  Improved documentation and the test suite.


Version 4.3 (November 4, 2021)
------------------------------

#)  Added methods :func:`dpiData_getJson()`,
    :func:`dpiData_getJsonArray()`,
    :func:`dpiData_getJsonObject()` to retrieve the value of the data
    when the native types are `DPI_NATIVE_TYPE_JSON`,
    `DPI_JSON_TYPE_JSON_ARRAY` and `DPI_NATIVE_TYPE_JSON_OBJECT` respectively.
#)  Added method :func:`dpiJson_setFromText()` to set a JSON value from a JSON
    string.
#)  Added method :func:`dpiVar_setFromJson()` to set the value of a JSON
    variable to a specified JSON value.
#)  Enhanced support for TPC (two-phase commit). The functions
    :func:`dpiConn_tpcBegin()`, :func:`dpiConn_tpcCommit()`,
    :func:`dpiConn_tpcEnd()`, :func:`dpiConn_tpcForget()`,
    :func:`dpiConn_tpcPrepare()` and :func:`dpiConn_tpcRollback()` were added.
    The functions dpiConn_beginDistribTrans() and dpiConn_prepareDistribTrans()
    are deprecated and will be removed in a future version.
#)  Added function :func:`dpiConn_setEcontextId()` to enable setting the
    execution context id attribute on connections.
#)  Added method :func:`dpiStmt_deleteFromCache()` to exclude the associated
    SQL statement from getting added to the statement cache.
#)  Added support for setting JSON node values using the following type
    combinations: `DPI_ORACLE_TYPE_NATIVE_DOUBLE` with `DPI_NATIVE_TYPE_DOUBLE`
    and `DPI_ORACLE_TYPE_NATIVE_FLOAT` with `DPI_NATIVE_TYPE_FLOAT`
    (`issue 155 <https://github.com/oracle/odpi/issues/155>`__).
#)  The method OCIAQDeq() is now used for single message dequeue in order to
    support dequeue of Oracle Transactional Event Queue (TEQ) messages.
#)  Added function :func:`dpiConn_getIsHealthy()` to do a local, light-weight
    connection health check.
#)  Added constant `DPI_ORACLE_TYPE_UROWID` as a synonym for
    `DPI_ORACLE_TYPE_ROWID` and placeholder for when it is possible to
    distinguish between ROWID and UROWID columns in the database.


Version 4.2.1 (June 1, 2021)
----------------------------

#)  Added support for caching the database version in pooled connections with
    Oracle Client 19 and earlier (later Oracle Clients handle this caching
    internally). This optimization eliminates a round-trip previously often
    required when reusing a pooled connection.
#)  Fixed a regression with error messages raised during connection creation.
#)  All errors identified as causing a dead connection now populate
    :member:`dpiErrorInfo.sqlState` with the value `01002` instead of only a
    hard-coded list of errors.
#)  Improved documentation and the test suite.


Version 4.2 (May 18, 2021)
--------------------------

#)  Added methods :func:`dpiPool_getMaxSessionsPerShard()`,
    :func:`dpiPool_getPingInterval()`,
    :func:`dpiPool_setMaxSessionsPerShard()`,
    :func:`dpiPool_setPingInterval()` and :func:`dpiPool_reconfigure()` in
    order to support changing pool configuration after the pool has been
    created.
#)  Added support for the SODA metadata cache available in Oracle Client
    21.3 and higher (also available in Oracle Client 19 from 19.11). This
    significantly improves the performance of repeated calls to methods
    :func:`dpiSodaDb_createCollection()` (when not specifying a value
    for the metadata parameter) and :func:`dpiSodaDb_openCollection()`. The
    member :member:`dpiCommonCreateParams.sodaMetadataCache` has been added and
    functions :func:`dpiPool_getSodaMetadataCache()` and
    :func:`dpiPool_setSodaMetadataCache()` have been added.
#)  Added support for supplying hints to SODA operations. The members
    :member:`dpiSodaOperOptions.hint` and
    :member:`dpiSodaOperOptions.hintLength` were added and methods
    :func:`dpiSodaColl_insertOneWithOptions()`,
    :func:`dpiSodaColl_insertManyWithOptions()` and
    :func:`dpiSodaColl_saveWithOptions()` were added. These can only be used
    with Oracle Client 21.3 and higher (also available in Oracle Client 19 from
    19.11).
#)  Added support for specifying the size of the statement cache when a pool or
    standalone connection is created. The member
    :member:`dpiCommonCreateParams.stmtCacheSize` was added and can be
    populated with the desired value prior to calling :func:`dpiPool_create()`
    or :func:`dpiConn_create()`.
#)  Added function :func:`dpiLob_getType()` in order to get the type of a
    LOB, as requested (`issue 135
    <https://github.com/oracle/odpi/issues/135>`__).
#)  Changed the requirement for the method :func:`dpiSodaColl_save()` to
    Oracle Client 19.9 or higher (instead of 20.1 or higher).
#)  Added flag to internal calls made by method
    :func:`dpiSodaColl_getDataGuide()` to ensure that the returned content
    is in encoding UTF-8. This ensures consistency with other SODA documents
    returned by ODPI-C.
#)  Corrected internal handling of flags sent to
    :func:`dpiSodaDocCursor_getNext()` and :func:`dpiSodaCollCursor_getNext()`.
#)  The distributed transaction handle assosciated with the connection is now
    cleared on commit or rollback (`cx_Oracle issue 530
    <https://github.com/oracle/python-cx_Oracle/issues/530>`__).
#)  When calling :func:`dpiJson_getValue()`, any cached value is now cleared
    before getting the new value in order to take into account possible calls
    to :func:`dpiJson_setValue()` that were made in between or to take into
    account the passing of different flags (`issue 154
    <https://github.com/oracle/odpi/issues/154>`__).
#)  Corrected internal handling of client version information when creating
    multiple contexts (`issue 156
    <https://github.com/oracle/odpi/issues/156>`__).
#)  Threaded mode is now always enabled when creating pools, regardless of what
    mode is provided in the :member:`dpiCommonCreateParams.createMode` member
    in order to provide for greater safety. Although there may be instances
    where threaded mode is not strictly needed, these are few and any
    advantages are minimal.
#)  Improved dead connection detection. If any error occurs that indicates that
    the connection is no longer usable, the connection is marked as dead and
    the unified error `DPI-1080: connection was closed by ORA-%d` (where the
    `%d` is replaced by the Oracle error that caused the connection to be
    closed) is returned instead. Attempts to use the connection after this
    result in the error `DPI-1010: not connected` being returned. This includes
    call timeout errors such as when :func:`dpiConn_setCallTimeout()` is called
    with a value set too low to allow the connection to break the executing
    statement and reset after the timeout occurs.
#)  Improved documentation and the test suite.


Version 4.1 (December 8, 2020)
------------------------------

#)  Added support for the new JSON data type available in Oracle Client and
    Database 21 and higher.
#)  Added methods :func:`dpiConn_getOciAttr()`, :func:`dpiConn_setOciAttr()`,
    :func:`dpiStmt_getOciAttr()` and :func:`dpiStmt_setOciAttr()` in order to
    be able to get and set OCI attributes that are otherwise not supported by
    ODPI-C. These methods should only be used as directed by Oracle.
#)  Workaround unexpected error when calling :func:`dpiLob_readBytes()` with a
    very small value in the `valueLength` parameter
    (`issue 146 <https://github.com/oracle/odpi/issues/146>`__).
#)  Ensure that calls to :func:`dpiStmt_setPrefetchRows()` are honored when
    binding as a REF cursor.
#)  Improved documentation and the test suite.


Version 4.0.2 (August 31, 2020)
-------------------------------

#)  Adjusted check for GNU version of strerror_r() on Cygwin as suggested
    (`issue 138 <https://github.com/oracle/odpi/issues/138>`__).
#)  Up to 40 digits can be represented in an unconstrained Oracle number so
    allow for that possibility (`cx_Oracle issue 459
    <https://github.com/oracle/python-cx_Oracle/issues/459>`__).
#)  Correct double free error
    (`issue 141 <https://github.com/oracle/odpi/issues/141>`__).
#)  Improved documentation and adjusted test suite.


Version 4.0.1 (June 26, 2020)
-----------------------------

#)  Ensure that all members of the ``dpiErrorInfo`` structure parameter are set
    properly during a call to :func:`dpiContext_getError()`.


Version 4.0 (June 25, 2020)
---------------------------

#)  Replaced function ``dpiContext_create()`` with
    :func:`dpiContext_createWithParams()`. A new structure
    :ref:`dpiContextCreateParams<dpiContextCreateParams>` can be passed to this
    function, enabling applications or drivers to modify how the Oracle Client
    library is loaded. In particular the structure allows the directories for
    Oracle Client libraries and configuration files to be specified. The
    original function ``dpiContext_create()`` is still available as a macro
    that calls the new function and passes NULL for the parameters (meaning all
    built-in defaults will be used).
#)  The default encoding for all character data is now UTF-8 but can be
    overridden by setting the member
    :member:`dpiContextCreateParams.defaultEncoding` before calling
    :func:`dpiContext_createWithParams()`. Setting the members
    :member:`dpiCommonCreateParams.encoding` and
    :member:`dpiCommonCreateParams.nencoding` to NULL will use the ``NLS_LANG``
    and ``NLS_NCHAR`` environment variables, which was the previous default.
#)  The algorithm by which the Oracle Client library is loaded was improved.
    See the :ref:`installation guide<installation>` for more details.
#)  Added new debugging level (64) for debugging the loading of the Oracle
    Client libraries (https://github.com/oracle/odpi/issues/131).
    See :ref:`debugging` for more details.
#)  Renamed debugging level 1 to ``DPI_DEBUG_LEVEL_UNREPORTED_ERRORS`` to be
    more clear as to its purpose.
#)  Added methods :func:`dpiSodaColl_save()` and :func:`dpiSodaColl_truncate()`
    available in Oracle Client 20 and higher.
#)  Added member :member:`dpiSodaOperOptions.fetchArraySize` which defines the
    array size to use when fetching SODA documents from a collection. It is
    available in Oracle Client 19.5 and higher.
#)  Internally make use of new mode available in Oracle Client 20.3 and higher
    in order to avoid a round-trip when calling
    :func:`dpiConn_getServerVersion()` with ``releaseString`` set to NULL.
#)  Added member :member:`dpiErrorInfo.isWarning` to identify warnings that are
    returned when calling :func:`dpiContext_getError()`. After a method
    returns ``DPI_SUCCESS``, a call to :func:`dpiContext_getError()` can be
    made to determine if a warning has been created. Calling any other function
    will result in the warning being cleared. Known warnings include
    ``ORA-28002: the password will expire within %d days`` (after a successful
    call to :func:`dpiPool_create()` or :func:`dpiConn_create()`) and
    ``ORA-24344: success with compilation error`` (after a successful call to
    :func:`dpiStmt_execute()` to create a stored procedure with compilation
    errors).
#)  Modified member :member:`dpiErrorInfo.offset` to be 32-bit and added new
    member :member:`dpiErrorInfo.offset16` for backwards compatibility; in this
    way row offsets that exceed 65536 can be reported successfully
    (`node-oracledb issue 1157
    <https://github.com/oracle/node-oracledb/issues/1157>`__).
#)  Added method :func:`dpiConn_startupDatabaseWithPfile()` in order to support
    starting up the database with a parameter file (PFILE), as requested
    (`issue 41 <https://github.com/oracle/odpi/issues/41>`__).
#)  Added support for converting all three timestamp types to/from double
    values (number of milliseconds since January 1, 1970) as used by frameworks
    such as Node.js.
#)  When an INSERT ALL statement is executed, return NULL in the ``rowid``
    parameter to :func:`dpiStmt_getLastRowid()` instead of the error
    ``DPI-1004: unable to get error message``.
#)  Added methods :func:`dpiStmt_getPrefetchRows()` and
    :func:`dpiStmt_setPrefetchRows()` in order to control how many rows the
    Oracle Client library prefetches when calling :func:`dpiStmt_execute()` for
    a query (`issue 73 <https://github.com/oracle/odpi/issues/73>`__).
#)  Added support for returning DATE values as ``DPI_NATIVE_TYPE_DOUBLE``, as
    commonly used by environments like Node.js
#)  Added DLL export attribute for Windows, as suggested by Kubo
    (`issue 126 <https://github.com/oracle/odpi/issues/126>`__).
#)  Improved documentation, samples, tutorial and test suite.


Version 3.3 (December 2, 2019)
------------------------------

#)  Added support for CQN and other subscription
    :member:`~dpiSubscrCreateParams.clientInitiated` connections to the
    database (as opposed to the default server initiated
    connections).
#)  Added function :func:`dpiStmt_getLastRowid()` for getting the rowid of the
    last row affected by a DML statement
    (`issue 111 <https://github.com/oracle/odpi/issues/111>`__).
#)  Added support for setting the
    :member:`~dpiPoolCreateParams.maxSessionsPerShard` attribute for session
    pools.
#)  Added support for providing double input for DATE sharding keys, as
    required by environments like Node.js.
#)  Corrected processing of DATE sharding keys (sharding requires a slightly
    different format to be passed to the server).
#)  Added support for using TIMESTAMP columns as sharding keys.
#)  Added check to ensure sharding key is specified when a super sharding key
    is specified.
#)  Improved error message when the library is loaded successfully but the
    attempt to detect the version of the library fails, either due to the fact
    that the library is too old or the method could not be called for some
    reason (`node-oracledb issue 1168
    <https://github.com/oracle/node-oracledb/issues/1168>`__).
#)  Adjusted support for creating a connection using an existing service
    context handle. The service context handle and its associated environment
    handle are now used directly in order to avoid potential memory corruption.
#)  Made the ``releaseString`` and ``releaseStringLength`` parameters to
    :func:`dpiConn_getServerVersion()` optional since they are not frequently
    used.
#)  Added ``ORA-3156: OCI call timed out`` to the list of error messages that
    result in error DPI-1067.
#)  Allow a NULL pointer to be passed to :func:`dpiVar_setFromBytes()`
    and :func:`dpiLob_setFromBytes()` when the length is zero.
#)  Improved documentation and test suite.


Version 3.2.2 (October 1, 2019)
-------------------------------

#)  Ensured that sharding keys are dedicated to the connection that is acquired
    using them in order to avoid possible hangs, crashes or unusual errors.
#)  Corrected support for PLS_INTEGER and BINARY_INTEGER types when used in
    PL/SQL records
    (`issue 112 <https://github.com/oracle/odpi/issues/112>`__).
#)  Improved documentation for :func:`dpiLob_getSize()`,
    :func:`dpiLob_readBytes()` and :func:`dpiLob_writeBytes()` regarding the
    lengths reported in number of characters
    (`issue 94 <https://github.com/oracle/odpi/issues/94>`__).


Version 3.2.1 (August 12, 2019)
-------------------------------

#)  A more meaningful error is now returned when calling
    :func:`dpiSodaColl_insertMany()` with an empty array.
#)  A more meaningful error is now returned when calling
    :func:`dpiSubscr_prepareStmt()` with SQL that is not a SELECT statement.
#)  Eliminated a segfault when calling :func:`dpiConn_close()` when a value was
    specified for the connection class during connection creation.
#)  Added documentation discussing round-trips to the database, as requested
    (`issue 108 <https://github.com/oracle/odpi/issues/108>`__).
#)  Improved processing of internal handle lists.


Version 3.2 (July 1, 2019)
--------------------------

#)  Added support for enqueue and dequeue of RAW payloads and for bulk enqueue
    and dequeue of messages. The methods dpiConn_deqObject() and
    dpiConn_enqObject() are deprecated and will be removed in version 4.0. The
    new methods :func:`dpiConn_newQueue()`, :func:`dpiQueue_deqMany()`,
    :func:`dpiQueue_deqOne()`, :func:`dpiQueue_enqMany()` and
    :func:`dpiQueue_enqOne()` should be used instead
    (`issue 58 <https://github.com/oracle/odpi/issues/58>`__ and
    `issue 104 <https://github.com/oracle/odpi/issues/104>`__).
#)  Added support for getting the registration id for a CQN subscription, as
    requested (`node-oracledb issue 1075
    <https://github.com/oracle/node-oracledb/issues/1075>`__).
#)  Removed preview status from existing SODA functionality. See
    `this tracking issue <https://github.com/oracle/odpi/issues/110>`__ for
    known issues with SODA.
#)  Added support for a preview of SODA bulk insert, available in Oracle Client
    18.5 and higher.
#)  Added support for setting the LOB prefetch length indicator in order to
    reduce the number of round trips when processing LOB values.
#)  Added support for getting and setting timestamp and date attributes of
    objects as double values (number of milliseconds since January 1, 1970), as
    preferred by some environments like Node.js.
#)  Added support for types BINARY_INTEGER, PLS_INTEGER, ROWID, LONG and LONG
    RAW when used in PL/SQL.
#)  Eliminated memory leak when fetching objects that are atomically null
    (`cx_Oracle issue 298
    <https://github.com/oracle/python-cx_Oracle/issues/298>`__).
#)  Eliminated memory leak when setting LOB attributes on objects.
#)  Eliminated bug when attempting to unregister a subscription while callbacks
    are ongoing.
#)  Eliminated bug when processing the string representation of numbers like
    1e-08 and 1e-09 (`cx_Oracle issue 300
    <https://github.com/oracle/python-cx_Oracle/issues/300>`__).
#)  Eliminated attempt to adjust the time a session was last used (to manage
    internal pool pinging functionality) if the pool is being closed.
#)  Eliminated potential segfault when an implicit result statement is closed
    before its parent statement is closed.
#)  Eliminated overhead by deferring the creation of OCI error handles until
    they are needed.
#)  Replaced prefix "Test" with "Demo" on all sample file names.
#)  Added additional test cases.
#)  Documentation improvements.


Version 3.1.4 (April 24, 2019)
------------------------------

#)  Added support for getting the row count for PL/SQL statements
    (`cx_Oracle issue 285
    <https://github.com/oracle/python-cx_Oracle/issues/285>`__).


Version 3.1.3 (March 12, 2019)
------------------------------

#)  Ensure that the strings "-0" and "-0.0" are correctly handled as zero
    values (`cx_Oracle issue 274
    <https://github.com/oracle/python-cx_Oracle/issues/274>`__).
#)  When using an external handle, create a new service context handle so that
    there are no discrepancies in character sets between the service context
    and the newly created environment handle (`cx_Oracle issue 273
    <https://github.com/oracle/python-cx_Oracle/issues/273>`__).
#)  Eliminated error when startup and shutdown events are generated
    (`issue 102 <https://github.com/oracle/odpi/issues/102>`__).
#)  Improved documentation.


Version 3.1.2 (February 19, 2019)
---------------------------------

#)  Corrected code for freeing CQN message objects when multiple queries are
    registered
    (`issue 96 <https://github.com/oracle/odpi/issues/96>`__).
#)  Fixed typo in documentation
    (`issue 97 <https://github.com/oracle/odpi/issues/97>`__).
#)  Improved error messages and installation documentation.


Version 3.1.1 (February 4, 2019)
--------------------------------

#)  Improved code preventing a statement from binding itself, in order to avoid
    a potential segfault under certain cirumstances.
#)  Always set :member:`dpiConnCreateParams.outNewSession` to the appropriate
    value when calling :func:`dpiPool_acquireConnection()` or
    :func:`dpiConn_create()`.
#)  Worked around OCI bug when attempting to free objects that are PL/SQL
    records, in order to avoid a potential segfault.
#)  Added samples demonstrating how to fetch CLOBs as strings and BLOBs as
    bytes (preferred for smaller LOBs).
#)  Documentation improvements based on feedback
    (`issue 87 <https://github.com/oracle/odpi/issues/87>`__,
    `issue 88 <https://github.com/oracle/odpi/issues/88>`__,
    `issue 90 <https://github.com/oracle/odpi/issues/90>`__,
    `issue 91 <https://github.com/oracle/odpi/issues/91>`__,
    `issue 92 <https://github.com/oracle/odpi/issues/92>`__).


Version 3.1 (January 21, 2019)
------------------------------

#)  Added support for getting and setting attributes of type RAW on Oracle
    objects, as requested
    (`issue 72 <https://github.com/oracle/odpi/issues/72>`__).
#)  Added function :func:`dpiData_getIsNull()` for getting the null indicator
    from a :ref:`dpiData <dpiData>` structure and function
    :func:`dpiData_setNull()` for setting a value to null, as requested
    (`issue 82 <https://github.com/oracle/odpi/issues/82>`__).
#)  Added support for fetching SYS.XMLTYPE objects as string data,
    limited to the VARCHAR2 length (`cx_Oracle issue 14
    <https://github.com/oracle/python-cx_Oracle/issues/14>`__).
#)  Added support for multi-property session tags and added attribute
    :member:`dpiPoolCreateParams.plsqlFixupCallback` for identifying a PL/SQL
    callback that will be executed when a requested tag doesn't match the
    tag associated with a pooled connection. Both of these features are only
    available in Oracle Client 12.2 and higher.
#)  The attribute :member:`dpiConnCreateParams.outNewSession` was added to
    support determining if a session in a pool is completely new and has never
    been acquired from the pool.
#)  Added support for performing external authentication with proxy for
    standalone connections.
#)  Added error message when external authentication with proxy is attempted
    without placing the user name in [] (proxy authentication is otherwise
    silently ignored).
#)  Exempted additional error messages from forcing the statement to be dropped
    from the cache (`issue 76 <https://github.com/oracle/odpi/issues/76>`__).
#)  Tightened up handling of numeric values when converted from a string
    representation. The error message returned when the string cannot be
    converted to an Oracle number was also improved.
#)  Completed enabling of pool pinging functionality for 12.2+ clients (see
    attribute :member:`dpiPoolCreateParams.pingInterval` for more information
    on this feature).
#)  Ensured that the connection returned from the pool after a failed ping
    (such as due to a killed session) is not itself marked as needing to be
    dropped.
#)  Eliminated memory leak under certain circumstances when pooled connections
    are released back to the pool.
#)  Eliminated memory leak when connections are dropped from the pool.
#)  Eliminated memory leak when calling :func:`dpiConn_close()` after fetching
    collections from the database.
#)  Adjusted order in which memory is freed when the last references to SODA
    collections, documents, document cursors and collection cursors are
    released, in order to prevent a segfault under certain circumstances.
#)  Improved error message when using an older version of the ODPI-C library.
#)  Added additional test cases.
#)  Improved documentation.


Version 3.0.0 (September 13, 2018)
----------------------------------

#)  Added support for Oracle Client 18 libraries.
#)  Added support for SODA (as preview). See
    :ref:`SODA Database<dpiSodaDbFunctions>`,
    :ref:`SODA Collection<dpiSodaCollFunctions>` and
    :ref:`SODA Document<dpiSodaDocFunctions>` for more information.
#)  Added support for call timeouts available in Oracle Client 18.1 and higher.
    See functions :func:`dpiConn_setCallTimeout()` and
    :func:`dpiConn_getCallTimeout()`.
#)  Added support for setting a LOB attribute of an object with string/bytes
    using the function :func:`dpiObject_setAttributeValue()`.
#)  Added support for the packed decimal type used by object attributes with
    historical types DECIMAL and NUMERIC
    (`cx_Oracle issue 212
    <https://github.com/oracle/python-cx_Oracle/issues/212>`__).
#)  On Windows, first attempt to load oci.dll from the same directory as the
    module that contains ODPI-C.
#)  SQL Objects that are created or fetched from the database are now tracked
    and marked unusable when a connection is closed. This was done in order to
    avoid a segfault in some circumstances.
#)  Improved support for closing pools by ensuring that once a pool has closed,
    further attempts to use connections acquired from that pool will fail with
    error "DPI-1010: not connected".
#)  Re-enabled pool pinging functionality for Oracle Client 12.2 and higher
    to handle classes of connection errors such as resource profile limits.
#)  Improved error messages when the Oracle Client or Oracle Database need to
    be at a minimum version in order to support a particular feature.
#)  Use plain integers instead of enumerations in order to simplify code and
    reduce the requirement for casts. Typedefs have been included so that code
    does not need to be changed.
#)  Eliminated potential buffer overrun
    (`issue 69 <https://github.com/oracle/odpi/issues/69>`__).
#)  In the Makefile for non-Windows platforms, the version information for
    ODPI-C is acquired directly from include/dpi.h as suggested
    (`issue 66 <https://github.com/oracle/odpi/issues/66>`__).
#)  Removed function dpiConn_newSubscription(). Use function
    :func:`dpiConn_subscribe()` instead.
#)  Removed function dpiLob_flushBuffer(). This function never worked anyway.
#)  Removed function dpiSubscr_close(). Use function
    :func:`dpiConn_unsubscribe()` instead.
#)  Removed function dpiVar_getData(). Use function
    :func:`dpiVar_getReturnedData()` instead.
#)  Added additional test cases.
#)  Improved documentation.


Version 2.4.2 (July 9, 2018)
----------------------------

#)  Avoid buffer overrun due to improper calculation of length byte when
    converting some negative 39 digit numbers from string to the internal
    Oracle number format
    (`issue 67 <https://github.com/oracle/odpi/issues/67>`__).


Version 2.4.1 (July 2, 2018)
----------------------------

#)  Use the install_name_tool on macOS to adjust the library name, as suggested
    (`issue 65 <https://github.com/oracle/odpi/issues/65>`__).
#)  Even when dequeue fails OCI still provides a message id so make sure it is
    deallocated to avoid a memory leak.
#)  Ensure that the row count for queries is reset to zero when the statement
    is executed (`cx_Oracle issue 193
    <https://github.com/oracle/python-cx_Oracle/issues/193>`__).
#)  If the statement should be deleted from the statement cache, first check to
    see that there is a statement cache currently being used; otherwise, the
    error "ORA-24300: bad value for mode" will be raised under certain
    conditions.


Version 2.4 (June 6, 2018)
--------------------------

#)  Added support for grouping events for subscriptions. See attributes
    :member:`dpiSubscrCreateParams.groupingClass`,
    :member:`dpiSubscrCreateParams.groupingValue` and
    :member:`dpiSubscrCreateParams.groupingType`.
#)  Added support for specifying the IP address a subscription should use
    instead of having the Oracle Client libraries determine the IP address to
    use on its own. See attributes
    :member:`dpiSubscrCreateParams.ipAddress` and
    :member:`dpiSubscrCreateParams.ipAddressLength`.
#)  Added support for subscribing to notifications when messages are available
    to dequeue in an AQ queue. See attribute
    :member:`dpiSubscrCreateParams.subscrNamespace` and the enumeration
    :ref:`dpiSubscrNamespace<dpiSubscrNamespace>` as well as the attributes
    :member:`dpiSubscrMessage.queueName`,
    :member:`dpiSubscrMessage.queueNameLength`,
    :member:`dpiSubscrMessage.consumerName` and
    :member:`dpiSubscrMessage.consumerNameLength`.
#)  Added attribute :member:`dpiSubscrMessage.registered` to allow the
    application to know when a subscription is no longer registered with the
    database. Deregistration can take place when the
    :member:`dpiSubscrCreateParams.timeout` value is reached or when
    :member:`dpiSubscrCreateParams.qos` is set to the value
    DPI_SUBSCR_QOS_DEREG_NFY. Note that notifications are not sent when a
    subscription is explicitly deregistered.
#)  Added method :func:`dpiConn_subscribe()` to replace method
    dpiConn_newSubscription() and added method :func:`dpiConn_unsubscribe()` to
    replace method dpiSubscr_close(). The replaced methods are deprecated and
    will be removed in version 3.0. The new methods clarify the fact that
    subscriptions do not require the connection they were created with to
    remain open. A new connection with the same credentials can be used to
    unusbscribe from events in the database.
#)  Added support for the pool "get" mode of timed wait. See attributes
    :member:`dpiPoolCreateParams.getMode` and
    :member:`dpiPoolCreateParams.waitTimeout`. The wait timeout value can be
    acquired after pool creation using the new method
    :func:`dpiPool_getWaitTimeout()` and set after pool creation using the new
    method :func:`dpiPool_setWaitTimeout()`.
#)  Added support for setting the maximum lifetime session and timeout
    parameters when creating a pool. See attributes
    :member:`dpiPoolCreateParams.maxLifetimeSession` and
    :member:`dpiPoolCreateParams.timeout`.
#)  Added support for installing ODPI-C into a user-defined prefix on platforms
    other than Windows, as requested
    (`issue 59 <https://github.com/oracle/odpi/issues/59>`__).
#)  Added support for setting the SONAME for shared libraries on platforms
    other than Windows, as requested
    (`issue 44 <https://github.com/oracle/odpi/issues/44>`__).
#)  Improved error message when attempting to create a subscription without
    enabling events mode when the pool or standalone connection is created.
#)  Added checks for minimal Oracle Client version (12.1) when calling the
    methods :func:`dpiPool_getMaxLifetimeSession()` and
    :func:`dpiPool_setMaxLifetimeSession()`.
#)  Added check to prevent attempts to bind PL/SQL array variables using the
    method :func:`dpiStmt_executeMany()`.
#)  Ensure that method :func:`dpiStmt_getRowCount()` returns the value 0 for
    all statements other than queries and DML, as documented.
#)  Correct handling of ROWIDs and statements when used as bind variables
    during execution of DML RETURNING statements.
#)  Added additional test cases.
#)  Improved documentation.


Version 2.3.2 (May 7, 2018)
---------------------------

#)  Ensure that a call to unregister a subscription only occurs if the
    subscription is still registered.
#)  Ensure that before a statement is executed that any dynamic buffers created
    for DML returning statments are reset since the out bind callback is not
    called if no rows are returned!
#)  Silenced compilation warning in test suite.
#)  Added test cases for DML returning statements.


Version 2.3.1 (April 25, 2018)
------------------------------

#)  Fixed determination of the number of rows returned in a DML Returning
    statement when the same statement is executed multiple times in succession
    with less rows being returned in each succeeding execution.
#)  Stopped attempting to unregister a CQN subscription before it was
    completely registered. This prevents errors encountered during registration
    from being masked by an error stating that the subscription has not been
    registered!
#)  Fixed support for true heterogeneous session pools that use different
    user/password combinations for each session acquired from the pool.
#)  Added error message indicating that modes DPI_MODE_EXEC_BATCH_ERRORS and
    DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS are only supported with insert, update,
    delete and merge statements.
#)  Corrected comment
    (`issue 61 <https://github.com/oracle/odpi/issues/61>`__).
#)  Renamed internal method dpiStmt__preFetch() to dpiStmt__beforeFetch() in
    order to avoid confusion with OCI prefetch.


Version 2.3 (April 2, 2018)
---------------------------

#)  Corrected support for getting the OUT values of bind variables bound to a
    DML Returning statement when calling the function
    :func:`dpiStmt_executeMany()`. Since multiple rows can be returned for each
    iteration, a new function :func:`dpiVar_getReturnedData()` has been added
    and the original function :func:`dpiVar_getData()` has been deprecated and
    will be removed in version 3.
#)  Corrected binding of LONG data (values exceeding 32KB) when using the
    function :func:`dpiStmt_executeMany()`.
#)  Added code to verify that the CQN subscription is open before permitting it
    to be used. Error "DPI-1060: subscription was already closed" will now be
    raised if an attempt is made to use a subscription that was closed earlier.
#)  Added error "DPI-1061: edition is not supported when a new password is
    specified" to clarify the fact that specifying an edition and a new
    password at the same time is not supported. Previously the edition value
    was simply ignored.
#)  Query metadata is no longer fetched if executing a statement with mode
    DPI_MODE_EXEC_PARSE_ONLY.
#)  Added additional statement types (DPI_STMT_TYPE_EXPLAIN_PLAN,
    DPI_STMT_TYPE_ROLLBACK and DPI_STMT_TYPE_COMMIT) as well as one that covers
    statement types not currently identified (DPI_STMT_TYPE_UNKNOWN).
#)  Improved error message when older OCI client libraries are being used that
    don't have the method OCIClientVersion().
#)  Corrected the handling of ANSI types REAL and DOUBLE PRECISION as
    implemented by Oracle. These types are just subtypes of NUMBER and are
    different from BINARY_FLOAT and BINARY_DOUBLE (`cx_Oracle issue 163
    <https://github.com/oracle/python-cx_Oracle/issues/163>`__).
#)  Added check that the return value from OCI functions matches the expected
    value of OCI_ERROR, and if not, raises an error including the value that
    was actually returned.
#)  Added additional test cases.
#)  Removed unused error messages.


Version 2.2.1 (March 5, 2018)
-----------------------------

#)  Maintain a reference to the "parent" object and use the actual object
    instance instead of a copy, so that "child" objects can be manipulated
    in-place instead of having to be created externally and then set
    (attributes) or appended (collections).
#)  Correct handling of boundary numbers 1e126 and -1e126.
#)  Eliminate memory leak when calling :func:`dpiConn_deqObject()` and
    :func:`dpiConn_enqObject()`.
#)  Eliminate memory leak when setting NCHAR and NVARCHAR attributes of
    objects.
#)  Eliminate memory leak when fetching collection objects from the database.
#)  Prevent internal re-execution of statement from duplicating itself in the
    list of open statements maintained on the connection.
#)  Improved documentation.


Version 2.2 (February 14, 2018)
-------------------------------

#)  Keep track of open statements and LOBs and automatically close them when
    the connection is closed; this eliminates the need for users of the driver
    to do so and removes the error "DPI-1054: connection cannot be closed when
    open statements or LOBs exist".
#)  Ignore failures that occur during the implicit rollback performed when a
    connection is closed, but if an error does occur, ensure that the
    connection is dropped from the pool (if it was acquired from a pool); such
    failures are generally due to an inability to communicate with the server
    (such as when your session has been killed).
#)  Avoid a round trip to the database when a connection is released back to
    the pool by preventing a rollback from being called when there is no
    transaction in progress.
#)  Improve error message when the use of bind variables is attempted with DDL
    statements, which is not supported.
#)  Since rowid is returned as a handle, the size in bytes and characters was
    simply being returned as the size of a pointer; set these values to 0
    instead as is done with other handles that are returned.
#)  Ensure that the LOB locator returned from the object attribute or element
    is not used directly as freeing it will result in unexpected behavior when
    the object containing it is itself freed.
#)  Make a copy of any objects that are acquired from other objects (either as
    attributes or elements of collections) in order to prevent possible use of
    the object acquired in such a fashion after the object it came from has
    been freed.
#)  Protect global variables on destruction as well since dpiGlobal__finalize()
    may not be the last method that is called if other methods are registered
    with atexit().
#)  Use cast to avoid assertions with isspace() when using debug libraries on
    Windows (`issue 52 <https://github.com/oracle/odpi/issues/52>`__).
#)  Added file embed/dpi.c to simplify inclusion of ODPI-C in projects.
#)  Minor changes to satisfy pickier compilers and static analysis tools.
#)  Added additional test cases.
#)  Improved documentation.


Version 2.1 (December 12, 2017)
-------------------------------

#)  Connections

    - Support was added for accessing sharded databases via sharding keys (new
      in Oracle 12.2). NOTE: the underlying OCI library has a bug when using
      standalone connections. There is a small memory leak proportional to the
      number of connections created/dropped. There is no memory leak when using
      session pools, which is recommended.
    - Added options for authentication with SYSBACKUP, SYSDG, SYSKM and SYSRAC,
      as requested (`cx_Oracle issue 101
      <https://github.com/oracle/python-cx_Oracle/issues/101>`__).
    - Attempts to release statements or free LOBs after the connection has been
      closed (by, for example, killing the session) are now prevented.
    - An error message was added when specifying an edition and a connection
      class since this combination is not supported.
    - Attempts to close the session for connections created with an external
      handle are now prevented.
    - Attempting to ping a database earlier than 10g results in ORA-1010:
      invalid OCI operation, but that implies a response from the database and
      therefore a successful ping, so treat it that way!
      (see `<https://github.com/rana/ora/issues/224>`__ for more information).

#)  Objects

    - Support was added for converting numeric values in an object type
      attribute to integer and text, as requested (`issue 35
      <https://github.com/oracle/odpi/issues/35>`__).
    - Methods :func:`dpiDeqOptions_setMsgId()` and
      :func:`dpiMsgProps_setOriginalMsgId()` now set their values correctly.
    - The overflow check when using double values as input to float attributes
      of objects or elements of collections was removed as it didn't work
      anyway and is a well-known issue that cannot be prevented without
      removing desired functionality. The developer should ensure that the
      source value falls within the limits of floats, understand the consequent
      precision loss or use a different data type.

#)  Variables

    - Support was added for setting a LOB variable using
      :func:`dpiVar_setFromBytes()`.
    - Added support for the case when the time zone minute offset is negative,
      as requested (`issue 38 <https://github.com/oracle/odpi/issues/38>`__).
    - Variables of type DPI_NATIVE_TYPE_BYTES are restricted to 2 bytes less
      than 1 GB (1,073,741,822 bytes), since OCI cannot handle more than that
      currently.

#)  Miscellaneous

    - Support was added for identifying the id of the transaction which spawned
      a CQN subscription message, as requested
      (`issue 32 <https://github.com/oracle/odpi/issues/32>`__).
    - Corrected use of subscription port number (`cx_Oracle issue 115
      <https://github.com/oracle/python-cx_Oracle/issues/115>`__).
    - Added support for getting information about MERGE statements, as
      requested (`issue 40 <https://github.com/oracle/odpi/issues/40>`__).
    - Problems reported with the usage of FormatMessage() on Windows was
      addressed (`issue 47 <https://github.com/oracle/odpi/issues/47>`__).
    - On Windows, if oci.dll cannot be loaded because it is the wrong
      architecture (32-bit vs 64-bit), attempt to find the offending DLL and
      include the full path of the DLL in the message, as suggested
      (`issue 49 <https://github.com/oracle/odpi/issues/49>`__).

#)  Debugging

    - Support was added to the debugging infrastructure to print the thread id
      and the date/time of messages. Support for an environment variable
      DPI_DEBUG_PREFIX was also added. See :ref:`debugging`.
    - Support was added for debugging both entry and exit points of ODPI-C
      public functions and for memory allocation/deallocation.

#)  Infrastructure

    - Dependent libraries were moved to the main Makefile so that applications
      do not have to do that, as suggested (`issue 33
      <https://github.com/oracle/odpi/issues/33>`__).
    - Added Makefile.win32 for the use of nmake on Windows and reworked
      existing Makefiles to support only platforms other than Windows.
    - Ensure that ODPI-C extended initialization code takes place before any
      other ODPI-C code can take place, and that it takes place in only one
      thread. Code was also added to cleanup the global OCI environment on
      process exit.
    - The OCI wrapers for using mutexes were eliminated, which improves
      performance.
    - Force OCI prefetch to always use the value 2; the OCI default is 1 but
      setting the ODPI-C default to 2 ensures that single row fetches don't
      require an extra round trip to determine if there are more rows to fetch;
      this change also reduces the potential memory consumption when
      fetchArraySize was set to a large value and also avoids performance
      issues discovered with larger values of prefetch.
    - Unused parameters for internal functions were removed where possible and
      ``__attribute((unused))`` added where not possible, as requested
      (`issue 39 <https://github.com/oracle/odpi/issues/39>`__).
    - The use of OCIThreadKeyInit() in any code other than the global
      initialization code was removed in order to avoid bugs in the OCI
      library.
    - Compiler warnings and Parfait warnings were eliminated.
    - Added additional test cases.
    - Documentation improvements.

#)  Deprecations

    - The function dpiLob_flushBuffer() is deprecated and will be removed in
      version 3. It previously always returned an error anyway because of the
      inability to enable LOB buffering and now always returns the error
      "DPI-1013: not supported".


Version 2.0.3 (November 6, 2017)
--------------------------------

#)  Prevent use of unitialized data in certain cases (`cx_Oracle issue 77
    <https://github.com/oracle/python-cx_Oracle/issues/77>`__).
#)  Attempting to ping a database earlier than 10g results in error "ORA-1010:
    invalid OCI operation", but that implies a response from the database and
    therefore a successful ping, so treat it that way!
#)  Some values represented as double do not convert perfectly to float; use
    FLT_EPSILON to check the difference between the two after conversion,
    rather than expect the values to convert perfectly.
#)  Prevent use of NaN with Oracle numbers since it produces corrupt data
    (`cx_Oracle issue 91
    <https://github.com/oracle/python-cx_Oracle/issues/91>`__).
#)  Verify that Oracle objects bound to cursors, fetched from cursors, set in
    object attributes or appended to collection objects are of the correct
    type.
#)  Correct handling of NVARCHAR2 when used as attributes of Oracle objects or
    elements of collections
    (`issue 45 <https://github.com/oracle/odpi/issues/45>`__).


Version 2.0.2 (August 30, 2017)
-------------------------------

#)  Don't prevent connection from being explicitly closed when a fatal error
    has taken place (`cx_Oracle issue 67
    <https://github.com/oracle/python-cx_Oracle/issues/67>`__).
#)  Correct handling of objects when dynamic binding is performed.
#)  Process deregistration events without an error.
#)  Eliminate memory leak when calling dpiObjectType_createObject().


Version 2.0.1 (August 18, 2017)
-------------------------------

#)  Ensure that any allocated statement increments the open child count
    (`issue 27 <https://github.com/oracle/odpi/issues/27>`__).
#)  Correct parameter subscrId to dpiConn_newSubscription() as
    `noted <https://github.com/oracle/odpi/issues/28>`__. There is no OCI
    documentation for this attribute and should never have been exposed. The
    parameter is therefore deprecated and will be removed in version 3. The
    value NULL can now also be passed to this parameter.
#)  Add script for running valgrind and correct issues found in test suite
    reported by valgrind
    (`issue 29 <https://github.com/oracle/odpi/issues/29>`__).
#)  Use posix_spawn() instead of system() in the test suite, as requested
    (`issue 30 <https://github.com/oracle/odpi/issues/30>`__).
#)  Add support for DML Returning statements that require dynamically allocated
    variable data (such as CLOBs being returned as strings).


Version 2.0.0 (August 14, 2017)
-------------------------------

#)  Added new structure :ref:`dpiDataTypeInfo<dpiDataTypeInfo>` and modified
    structures :ref:`dpiQueryInfo<dpiQueryInfo>`,
    :ref:`dpiObjectAttrInfo<dpiObjectAttrInfo>` and
    :ref:`dpiObjectTypeInfo<dpiObjectTypeInfo>` in order to add support for
    fractional seconds precision on timestamps and interval day to second
    values (`issue 22 <https://github.com/oracle/odpi/issues/22>`__) and to
    support additional metadata on object types and attributes
    (`issue 23 <https://github.com/oracle/odpi/issues/23>`__).
#)  Prevent closing the connection when there are any open statements or LOBs
    and add new error "DPI-1054: connection cannot be closed when open
    statements or LOBs exist" when this situation is detected; this is needed
    to prevent crashes under certain conditions when statements or LOBs are
    being acted upon while at the same time (in another thread) a connection is
    being closed; it also prevents leaks of statements and LOBs when a
    connection is returned to a session pool.
#)  Added support for binding by value for rowids in
    :func:`dpiStmt_bindValueByPos()` and :func:`dpiStmt_bindValueByName()`.
#)  On platforms other than Windows, if the regular method for loading the
    Oracle Client libraries fails, try using $ORACLE_HOME/lib/libclntsh.so
    (`issue 20 <https://github.com/oracle/odpi/issues/20>`__).
#)  Use the environment variable DPI_DEBUG_LEVEL at runtime, not compile time,
    to add in :ref:`debugging <debugging>`.
#)  Added support for DPI_DEBUG_LEVEL_ERRORS (reports errors and has the value
    8) and DPI_DEBUG_LEVEL_SQL (reports prepared SQL statement text and has the
    value 16) in order to further improve the ability to debug issues.
#)  Ensure that any prefetch activities are performed prior to performing the
    fetch in :func:`dpiStmt_scroll()`.
#)  Provide means of disabling thread cleanup for situations where threads are
    created at startup and never terminated (such as takes place with Node.js).
#)  Add script sql/TestEnv.sql and read environment variables during test
    execution in order to simplify the running of tests.
#)  Add script sql/SampleEnv.sql and read environment variables during sample
    execution in order to simplify the running of samples.
#)  Adjust Makefile for tests and samples
    (`issue 24 <https://github.com/oracle/odpi/issues/24>`__).
#)  Added additional test cases.
#)  Documentation improvements.


Version 2.0.0-rc.2 (July 20, 2017)
----------------------------------

#)  Improved error messages when initial OCI environment cannot be created.
#)  On Windows, convert system message to Unicode first, then to UTF-8;
    otherwise, the error message returned could be in a mix of encodings.
#)  Added support for converting from text to number in object attributes and
    collection element values.
#)  Added checks on all pointers to ensure they are not NULL.
#)  Added checks on all pointer/length combinations to ensure that they are not
    NULL and non-zero at the same time.
#)  Ensure that the thread specific errors are freed properly.
#)  Corrected handling of connections using DRCP.
#)  Corrected issue where error getting a pooled connection was masked by
    error "DPI-1002: invalid OCI handle".
#)  PL/SQL boolean values are not supported until Oracle Client 12.1 so raise
    an unsupported error if an attempt is made to use them in Oracle Client
    11.2.
#)  Allow the parameter numQueryColumns to be null in :func:`dpiStmt_execute()`
    as a convenience to those who do not require that information.
#)  Added url fragment and whether a 32-bit or 64-bit library is expected to
    error "DPI-1047: Oracle Client library cannot be loaded" in order to
    improve the help provided.
#)  Added prefix "ODPI: " to start of all debug messages to aid in
    differentiating between them and other messages in log files.
#)  Added additional test cases.
#)  Documentation improvements.


Version 2.0.0-rc.1 (June 16, 2017)
----------------------------------

#)  OCI requires that both :member:`~dpiCommonCreateParams.encoding`
    and :member:`~dpiCommonCreateParams.nencoding` have values or that both
    encoding and encoding do not have values. The missing value is set to its
    default value if one of the values is set and the other is not
    (`issue 36 <https://github.com/oracle/python-cx_Oracle/issues/36>`__).
#)  Add optimization when client and server character sets are identical; in
    that case the size in bytes reported by the server is sufficient to hold
    the data that will be transferred from the server and there is no need to
    expand the buffer for character strings.
#)  Corrected handling of BFILE LOBs.
#)  Eliminated errors and warnings when using AIX compiler.
#)  Documentation improvements.


Version 2.0.0-beta.4 (May 24, 2017)
-----------------------------------

#)  Added support for getting/setting attributes of objects or element values
    in collections that contain LOBs, BINARY_FLOAT values, BINARY_DOUBLE values
    and NCHAR and NVARCHAR2 values. The error message for any types that are
    not supported has been improved as well.
#)  Enabled temporary LOB caching in order to avoid disk I/O as
    `suggested <https://github.com/oracle/odpi/issues/10>`__.
#)  Changed default native type to DPI_ORACLE_TYPE_INT64 if the column metadata
    indicates that the values are able to fit inside a 64-bit integer.
#)  Added function :func:`dpiStmt_defineValue()`, which gives the application
    the opportunity to specify the data type to use for fetching without having
    to create a variable.
#)  Added constant DPI_DEBUG_LEVEL as a set of bit flags which result in
    messages being printed to stderr. The following levels are defined:

    - 0x0001 - reports errors during free operations
    - 0x0002 - reports on reference count changes
    - 0x0004 - reports on public function calls

#)  An empty string is just as acceptable as NULL when enabling external
    authentication in :func:`dpiPool_create()`.
#)  Avoid changing the OCI actual length values for fixed length types in order
    to prevent error "ORA-01458: invalid length inside variable character
    string".
#)  Ensured that the length set in the dpiBytes structure by the caller is
    passed through to the actual length buffer used by OCI.
#)  Added missing documentation for function :func:`dpiVar_setFromBytes()`.
#)  Handle edge case when an odd number of zeroes trail the decimal point in a
    value that is effectively zero (`cx_Oracle issue 22
    <https://github.com/oracle/python-cx_Oracle/issues/22>`__).
#)  Eliminated resource leak when a standalone connection or pool is freed.
#)  Prevent attempts from binding the cursor being executed to itself.
#)  Corrected determination of unique bind variable names. The function
    :func:`dpiStmt_getBindCount()` returns a count of unique bind variable
    names for PL/SQL statements only. For SQL statements, this count is the
    total number of bind variables, including duplicates. The function
    :func:`dpiStmt_getBindNames()` has been adjusted to return the actual
    number of unique bind variable names (parameter numBindNames is now a
    pointer instead of a scalar value).
#)  Added additional test cases.
#)  Added check for Cygwin, as `suggested
    <https://github.com/oracle/odpi/issues/11>`__.


Version 2.0.0-beta.3 (April 18, 2017)
-------------------------------------

#)  Add initial set of `functional test cases
    <https://github.com/oracle/odpi/tree/main/test>`__.
#)  Add support for smallint and float data types in Oracle objects, as
    `requested <https://github.com/oracle/python-cx_Oracle/issues/4>`__.
#)  Ensure that the actual array size is set to the number of rows returned in
    a DML Returning statement.
#)  Remove unneeded function dpiVar_resize().
#)  Improve error message when specifying an invalid array position in a
    variable.
#)  Add structure :ref:`dpiVersionInfo<dpiVersionInfo>` to pass version
    information, rather than separate parameters. This affects functions
    :func:`dpiContext_getClientVersion()` and
    :func:`dpiConn_getServerVersion()`.
#)  Rename functions that use an index to identify elements in a collection to
    include "ByIndex" in the name. This is clearer and also allows for
    functions that may be added in the future that will identify elements by
    other means. This affects functions
    :func:`dpiObject_deleteElementByIndex()`,
    :func:`dpiObject_getElementExistsByIndex()`,
    :func:`dpiObject_getElementValueByIndex()`, and
    :func:`dpiObject_setElementValueByIndex()`.
#)  The OCI function OCITypeByFullName() is supported on a 12.1 Oracle Client
    but will give the error "OCI-22351: This API is not supported by the ORACLE
    server" when used against an 11.2 Oracle Database. The function
    :func:`dpiConn_getObjectType()` now checks the server version and calls the
    correct routine as needed.
#)  Add parameter "exists" to functions :func:`dpiObject_getFirstIndex()` and
    :func:`dpiObject_getLastIndex()` which allow a calling program to avoid
    error "OCI-22166: collection is empty".


Version 2.0.0-beta.2 (March 28, 2017)
-------------------------------------

#)  Use dynamic loading at runtime to load the OCI library and eliminate the
    need for the OCI header files to be present when building ODPI-C.
#)  Improve sample Makefile as requested in `issue 1
    <https://github.com/oracle/odpi/issues/1>`__.
#)  Correct support for handling unsigned integers that are larger than the
    maximum size that can be represented by a signed integer. This corrects
    `issue 3 <https://github.com/oracle/odpi/issues/3>`__.
#)  Remove type DPI_ORACLE_TYPE_LONG_NVARCHAR which is not needed as noted in
    `issue 5 <https://github.com/oracle/odpi/issues/5>`__.
#)  Increase size of string which can be generated from an OCI number. This
    corrects `issue 6 <https://github.com/oracle/odpi/issues/6>`__.
#)  Ensure that zeroing the check integer on ODPI-C handles is not optimised
    away by the compiler.
#)  Silence compiler warnings from the Microsoft C++ compiler.
#)  Restore support for simple reference count tracing by the use of
    DPI_TRACE_REFS.
#)  Add additional error (ORA-56600: an illegal OCI function call was issued)
    to the list of errors that cause the session to be dropped from the session
    pool.
#)  Changed LOB sample to include code to populate both CLOBs and BLOBs in
    addition to fetching them.


Version 2.0.0-beta.1 (January 30, 2017)
---------------------------------------

#)  Initial release supporting the following features\:

    - 11.2, 12.1 and 12.2 Oracle Client support
    - 9.2 and higher Oracle Database support (depending on Oracle Client
      version)
    - SQL and PL/SQL execution
    - Character datatypes (CHAR, VARCHAR2, NCHAR, NVARCHAR2, CLOB, NCLOB, LONG)
    - Numeric datatypes (NUMBER, BINARY_FLOAT, BINARY_DOUBLE)
    - Dates, Timestamps, Intervals
    - Binary types (BLOB, BFILE, RAW, LONG RAW)
    - PL/SQL datatypes (PLS_INTEGER, BOOLEAN, Collections, Records)
    - JSON
    - User Defined Types
    - REF CURSOR, Nested cursors, Implicit Result Sets
    - Array fetch
    - Array bind/execute
    - Session pools (homogeneous and non-homogeneous with proxy authentication)
    - Standalone connections
    - Database Resident Connection Pooling (DRCP)
    - External authentication
    - Statement caching (tagging)
    - End-to-end tracing, mid-tier authentication and auditing (action, module,
      client identifier, client info, database operation)
    - Edition Based Redefinition
    - Scrollable cursors
    - DML RETURNING
    - Privileged connection support (SYSDBA, SYSOPER, SYSASM, PRELIM_AUTH)
    - Database Startup/Shutdown
    - Session Tagging
    - Proxy authentication
    - Batch Errors
    - Array DML Row Counts
    - Query Result Caching
    - Application Continuity (with some limitations)
    - Query Metadata
    - Password Change
    - Two Phase Commit
    - OCI Client Version and Server Version
    - Connection Validation (when acquired from session pool or DRCP)
    - Continuous Query Notification
    - Advanced Queuing
    - Easily extensible (via direct OCI calls)
