//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2025, Oracle and/or its affiliates.
//
// This software is dual-licensed to you under the Universal Permissive License
// (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
// 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
// either license.
//
// If you elect to accept the software under the Apache License, Version 2.0,
// the following applies:
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// test_2800_lobs.c
//   Test suite for testing all the LOB related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define DEFAULT_CHARS                   "abcdef"
#define MAX_CHARS                       200


//-----------------------------------------------------------------------------
// dpiTest__populateAndGetLobFromTable() [INTERNAL]
//   Function to insert LOB and select LOB from corresponding LOB tables.
//-----------------------------------------------------------------------------
int dpiTest__populateAndGetLobFromTable(dpiTestCase *testCase, dpiConn *conn,
        dpiOracleTypeNum oracleTypeNum, const char *value,
        uint32_t valueLength, dpiLob **lob)
{
    dpiOracleTypeNum varOracleTypeNum, lobOracleTypeNum;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *varData, *tempData;
    uint32_t bufferRowIndex;
    const char *lobType;
    char sql[100];
    dpiStmt *stmt;
    dpiVar *var;
    int found;

    // verify type of LOB
    switch (oracleTypeNum) {
        case DPI_ORACLE_TYPE_CLOB:
            lobType = "CLOB";
            varOracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
            break;
        case DPI_ORACLE_TYPE_NCLOB:
            varOracleTypeNum = DPI_ORACLE_TYPE_NVARCHAR;
            lobType = "NCLOB";
            break;
        case DPI_ORACLE_TYPE_BLOB:
            varOracleTypeNum = DPI_ORACLE_TYPE_RAW;
            lobType = "BLOB";
            break;
        default:
            return dpiTestCase_setFailed(testCase, "invalid LOB type");
    }

    // use default values if none supplied
    if (!value) {
        value = DEFAULT_CHARS;
        valueLength = strlen(value);
    }

    // truncate table
    sprintf(sql, "truncate table Test%ss", lobType);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row into the database
    sprintf(sql, "insert into Test%ss values (1, :1)", lobType);
    if (dpiConn_newVar(conn, varOracleTypeNum, DPI_NATIVE_TYPE_BYTES, 1,
            valueLength, 1, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(var, 0, value, valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform query
    sprintf(sql, "select * from Test%ss for update", lobType);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase, "no rows found");
    if (dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    *lob = tempData->value.asLOB;
    if (dpiLob_addRef(*lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getType(*lob, &lobOracleTypeNum) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, lobOracleTypeNum,
            oracleTypeNum) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyLobWithGivenSize() [INTERNAL]
//   Function to fetch LOB and check their sizes.
//-----------------------------------------------------------------------------
int dpiTest__verifyLobWithGivenSize(dpiTestCase *testCase, dpiConn *conn,
        uint32_t lobSize, dpiOracleTypeNum oracleTypeNum)
{
    const char alphaNum[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    uint64_t bufferSize, numBytes, i;
    char *readBuffer, *writeBuffer;
    dpiLob *lob;

    // write random set of bytes to a buffer and populate LOB from that buffer
    writeBuffer = malloc(lobSize);
    if (!writeBuffer)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    for (i = 0; i < lobSize; i++)
        writeBuffer[i] = alphaNum[rand() % (sizeof(alphaNum) - 1)];
    if (dpiTest__populateAndGetLobFromTable(testCase, conn, oracleTypeNum,
            writeBuffer, lobSize, &lob) < 0)
        return DPI_FAILURE;

    // read the data from that LOB and verify it matches what was written
    if (dpiLob_getBufferSize(lob, lobSize, &bufferSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    readBuffer = malloc(bufferSize);
    if (!readBuffer)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    numBytes = bufferSize;
    if (dpiLob_readBytes(lob, 1, lobSize, readBuffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, readBuffer, numBytes,
            writeBuffer, lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    free(readBuffer);
    free(writeBuffer);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2800()
//   Call dpiConn_newTempLob() for lobType values of DPI_ORACLE_TYPE_CLOB,
// DPI_ORACLE_TYPE_BLOB and DPI_ORACLE_TYPE_NCLOB (no error).
//-----------------------------------------------------------------------------
int dpiTest_2800(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_BLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_NCLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2801()
//   Call dpiConn_newTempLob() for an invalid lob type value (error DPI-1021).
//-----------------------------------------------------------------------------
int dpiTest_2801(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_VARCHAR, &lob);
    return dpiTestCase_expectError(testCase, "DPI-1021");
}


//-----------------------------------------------------------------------------
// dpiTest_2802()
//   Call dpiConn_newTempLob() for a valid lob type; call dpiLob_release()
// twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2802(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_release(lob);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_2803()
//   Call dpiConn_newTempLob() for a valid lob type; call dpiLob_close(); call
// each of the public LOB functions except dpilob_addRef() and dpiLob_release()
// (error DPI-1040).
//-----------------------------------------------------------------------------
int dpiTest_2803(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1040:";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_close(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_copy(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getBufferSize(lob, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getChunkSize(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, NULL, NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getIsResourceOpen(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getSize(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_openResource(lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_closeResource(lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_readBytes(lob, 0, 0, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, NULL, 0, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_setFromBytes(lob, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_trim(lob, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_writeBytes(lob, 0, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2804()
//   Call dpiConn_newTempLob() for a valid lob type; call
// dpiLob_closeResource() (error).
//-----------------------------------------------------------------------------
int dpiTest_2804(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_closeResource(lob);
    if (dpiTestCase_expectError(testCase, "ORA-22289:") < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2805()
//   Call dpiConn_newTempLob() for a valid lob type; call dpiLob_openResource()
// (no error).
//-----------------------------------------------------------------------------
int dpiTest_2805(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2806()
//   Prepare and execute a select for update statment that returns a LOB and
// fetch one of the LOBs; call dpiLob_closeResource() (error).
//-----------------------------------------------------------------------------
int dpiTest_2806(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_closeResource(lob);
    if (dpiTestCase_expectError(testCase, "ORA-22289:") < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2807()
//   Prepare and execute a select for update statment that returns a LOB and
// fetch one of the LOBs; call dpiLob_openResource(); call dpiLob_writeBytes()
// with any value; call dpiConn_commit() (error).
//-----------------------------------------------------------------------------
int dpiTest_2807(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 1, "test", strlen("test")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_commit(conn);
    if (dpiTestCase_expectError(testCase, "ORA-22297:") < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2808()
//   Prepare and execute a select for update statment that returns a LOB and
// fetch one of the LOBs; call dpiLob_openResource(); call dpiLob_writeBytes()
// with any value; call dpiLob_closeResource(); call dpiConn_commit() (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_2808(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 1, "test", strlen("test")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_closeResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2809()
//   Fetch a CLOB from the database; call dpiLob_getBufferSize() and verify
// that the size in bytes returned is a multiple of the maxBytesPerCharacter
// value in the connection encoding information structure (no error).
//-----------------------------------------------------------------------------
int dpiTest_2809(dpiTestCase *testCase, dpiTestParams *params)
{
    uint64_t lobSize, sizeInBytes;
    dpiEncodingInfo info;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getBufferSize(lob, lobSize, &sizeInBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, sizeInBytes,
            lobSize * info.maxBytesPerCharacter) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2810()
//   Fetch a NCLOB from the database; call dpiLob_getBufferSize() and verify
// that the size in bytes returned is a multiple of the nmaxBytesPerCharacter
// value in the connection encoding information structure (no error).
//-----------------------------------------------------------------------------
int dpiTest_2810(dpiTestCase *testCase, dpiTestParams *params)
{
    uint64_t lobSize, sizeInBytes;
    dpiEncodingInfo info;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getBufferSize(lob, lobSize, &sizeInBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, sizeInBytes,
            lobSize * info.nmaxBytesPerCharacter) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2811()
//   Fetch a BLOB from the database; call dpiLob_getBufferSize() and verify
// that the size in bytes returned is identical to the sizeInChars value that
// was passed (no error).
//-----------------------------------------------------------------------------
int dpiTest_2811(dpiTestCase *testCase, dpiTestParams *params)
{
    uint64_t lobSize, sizeInBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getBufferSize(lob, lobSize, &sizeInBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, sizeInBytes, lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2812()
//   Fetch a LOB from the database; call dpiLob_getChunkSize() and verify that
// the value returned matches the expected value (no error).
//-----------------------------------------------------------------------------
int dpiTest_2812(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    uint32_t size;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getChunkSize(lob, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase,
            (16384 - size) & 0xfffffe00, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2813()
//   Fetch a CLOB, NCLOB or BLOB from the database; call the function
// dpiLob_getDirectoryAndFileName() (error).
//-----------------------------------------------------------------------------
int dpiTest_2813(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";
    uint32_t directoryAliasLength, fileNameLength;
    const char *directoryAlias, *fileName;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, &directoryAlias, &directoryAliasLength,
            &fileName, &fileNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, &directoryAlias, &directoryAliasLength,
            &fileName, &fileNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, &directoryAlias, &directoryAliasLength,
            &fileName, &fileNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2814()
//   Fetch a CLOB, NCLOB or BLOB from the database; call the function
// dpiLob_getFileExists() (error).
//-----------------------------------------------------------------------------
int dpiTest_2814(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";
    dpiConn *conn;
    dpiLob *lob;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2815()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_getIsResourceOpen() and verify that the
// value returned is 0; call dpiLob_openResource(); call
// dpiLob_getIsResourceOpen() a second time and verify that the value returned
// is now 1 (no error).
//-----------------------------------------------------------------------------
int dpiTest_2815(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;
    int isOpen;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getIsResourceOpen(lob, &isOpen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isOpen, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getIsResourceOpen(lob, &isOpen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isOpen, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_closeResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2816()
//   Fetch a LOB from the database; call dpiLob_getSize() and verify that the
// value returned matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_2816(dpiTestCase *testCase, dpiTestParams *params)
{
    uint64_t lobSize;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, lobSize, 6) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2817()
//   Fetch a CLOB, NCLOB or BLOB from the database; call
// dpiLob_setDirectoryAndFileName() (error).
//-----------------------------------------------------------------------------
int dpiTest_2817(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "OCI-22275:";
    const char *dirName = "X", *fileName = "garbage.txt";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, dirName, strlen(dirName), fileName,
            strlen(fileName));
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, dirName, strlen(dirName), fileName,
            strlen(fileName));
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, dirName, strlen(dirName), fileName,
            strlen(fileName));
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2818()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_trim() with a value greater than the
// current length of the LOB (error).
//-----------------------------------------------------------------------------
int dpiTest_2818(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_trim(lob, 30);
    if (dpiTestCase_expectError(testCase, "ORA-22926:") < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2819()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_trim() with a value smaller than the
// current length of the LOB and verify that the new size matches the size
// that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2819(dpiTestCase *testCase, dpiTestParams *params)
{
    uint64_t lobSize;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_trim(lob, 3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, lobSize, 3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2820()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_writeBytes() with any value; call
// dpiLob_copy() and verify that the values in the LOBs are identical; call
// dpiLob_writeBytes() on one of the LOBs and verify that the values in the
// other LOB is unchanged (no error).
//-----------------------------------------------------------------------------
int dpiTest_2820(dpiTestCase *testCase, dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    dpiLob *lob, *copyLob;
    uint64_t numBytes;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_writeBytes(lob, 1, "copying", strlen("copying")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_copy(lob, &copyLob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(copyLob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "copying",
            strlen("copying")) < 0)
        return DPI_FAILURE;
    if (dpiLob_writeBytes(copyLob, 1, "Testing", strlen("Testing")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "copying",
            strlen("copying")) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(copyLob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2821()
//   Fetch a CLOB from the database; call dpiLob_readBytes() at a number of
// offsets (beginning, end, somewhere in the middle) and verify that the
// data returned matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_2821(dpiTestCase *testCase, dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "abcdef",
            strlen("abcdef")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 3, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "cdef",
            strlen("cdef")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 8, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "", 0) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2822()
//   Fetch a NCLOB from the database; call dpiLob_readBytes() at a number of
// offsets (beginning, end, somewhere in the middle) and verify that the data
// returned matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_2822(dpiTestCase *testCase, dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "abcdef",
            strlen("abcdef")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 4, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "def",
            strlen("def")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 8, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "", 0) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2823()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_setFromBytes() with a value that is
// smaller than the original size of the LOB and verify that the new value
// of the LOB is correct (no error)
//-----------------------------------------------------------------------------
int dpiTest_2823(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *value = "Test";
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_setFromBytes(lob, value, strlen(value)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, value,
            strlen(value)) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2824()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_setFromBytes() with a value that is
// larger than the original size of the LOB and verify that the new value of
// the LOB is correct (no error).
//-----------------------------------------------------------------------------
int dpiTest_2824(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *value = "a much longer LOB value than previously";
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_setFromBytes(lob, value, strlen(value)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, value,
            strlen(value)) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2825()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_writeBytes() for a number of offsets
// (beginning, middle and end of the LOB) and after commiting the transaction
// perform a second fetch and verify that the LOB contents match what was
// written (no error).
//-----------------------------------------------------------------------------
int dpiTest_2825(dpiTestCase *testCase, dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_writeBytes(lob, 1, "lob", strlen("lob")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 4, "test", strlen("test")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 8, "ing", strlen("ing")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes,
                "lobtesting", strlen("lobtesting")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2826()
//   Fetch LOBs (CLOB, NCLOB, BLOB) from the database using a variable with
// native type DPI_NATIVE_TYPE_BYTES; verify that the value returned matches
// the expected value for each of the below combinations (no error):
//     LOB of 64K - 1 bytes
//     LOB of 64K bytes
//     LOB of 64K + 1 bytes
//     LOB of 512K bytes
//     LOB of 512K + 1 bytes
//     LOB of 1024K bytes
//     LOB of 1024K + 1 bytes
//-----------------------------------------------------------------------------
int dpiTest_2826(dpiTestCase *testCase, dpiTestParams *params)
{
    const uint32_t sizes[7] = { 65535, 65536, 65537, 524288, 524289, 1048576,
            1048577 };
    const dpiOracleTypeNum oracleTypeNums[3] = { DPI_ORACLE_TYPE_CLOB,
            DPI_ORACLE_TYPE_NCLOB, DPI_ORACLE_TYPE_BLOB };
    dpiConn *conn;
    int i, j;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 7; j++) {
            if (dpiTest__verifyLobWithGivenSize(testCase, conn, sizes[j],
                    oracleTypeNums[i]) < 0)
                return DPI_FAILURE;
        }
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2827()
//   Verify dpiLob_setFromBytes() allows value parameter to be NULL when the
// valueLength parameter has the value zero.
//-----------------------------------------------------------------------------
int dpiTest_2827(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_setFromBytes(lob, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2828()
//   Verify dpiLob_setFromBytes() does not allow value parameter to be NULL
// when the valueLength parameter has a non-zero value.
//-----------------------------------------------------------------------------
int dpiTest_2828(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_setFromBytes(lob, NULL, 1);
    if (dpiTestCase_expectError(testCase, "DPI-1053:") < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2800);
    dpiTestSuite_addCase(dpiTest_2800,
            "create all types of LOBs");
    dpiTestSuite_addCase(dpiTest_2801,
            "create invalid LOB type");
    dpiTestSuite_addCase(dpiTest_2802,
            "call dpiLob_release() twice");
    dpiTestSuite_addCase(dpiTest_2803,
            "call all LOB public functions after closing the lob");
    dpiTestSuite_addCase(dpiTest_2804,
            "call dpiLob_closeResource on unopened lob");
    dpiTestSuite_addCase(dpiTest_2805,
            "call dpiLob_openResource and verify");
    dpiTestSuite_addCase(dpiTest_2806,
            "fetch a lob and call dpiLob_closeResource");
    dpiTestSuite_addCase(dpiTest_2807,
            "verify commit on a unclosed lob returns error");
    dpiTestSuite_addCase(dpiTest_2808,
            "verify commit on a closed lob");
    dpiTestSuite_addCase(dpiTest_2809,
            "verify getBufferSize on Clob returns the expected value");
    dpiTestSuite_addCase(dpiTest_2810,
            "verify getBufferSize on Nclob returns the expected value");
    dpiTestSuite_addCase(dpiTest_2811,
            "verify getBufferSize on Blob returns the expected value");
    dpiTestSuite_addCase(dpiTest_2812,
            "verify getChunkSize returns the expected value");
    dpiTestSuite_addCase(dpiTest_2813,
            "verify getDirectoryAndFileName func with CLOB, NCLOB, BLOB");
    dpiTestSuite_addCase(dpiTest_2814,
            "verify getFileExists func with CLOB, NCLOB, BLOB");
    dpiTestSuite_addCase(dpiTest_2815,
            "verify dpiLob_getIsResourceOpen works as expected");
    dpiTestSuite_addCase(dpiTest_2816,
            "verify dpiLob_getSize works as expected");
    dpiTestSuite_addCase(dpiTest_2817,
            "verify setDirectoryAndFileName func with CLOB, NCLOB, BLOB");
    dpiTestSuite_addCase(dpiTest_2818,
            "verify trim function with bigger value than existing length");
    dpiTestSuite_addCase(dpiTest_2819,
            "verify trim function with smaller value than existing length");
    dpiTestSuite_addCase(dpiTest_2820,
            "verify dpiLob_copy works as expected ");
    dpiTestSuite_addCase(dpiTest_2821,
            "verify readBytes on Clob with diff offsets works as expected");
    dpiTestSuite_addCase(dpiTest_2822,
            "verify readBytes on Nclob with diff offsets works as expected");
    dpiTestSuite_addCase(dpiTest_2823,
            "verify setFromBytes with small value than existing and verify");
    dpiTestSuite_addCase(dpiTest_2824,
            "verify setFromBytes with big value than existing and verify");
    dpiTestSuite_addCase(dpiTest_2825,
            "verify writeBytes on Clob with diff offsets works as expected");
    dpiTestSuite_addCase(dpiTest_2826,
            "verify CLOB, NCLOB, BLOB values with different buffer sizes");
    dpiTestSuite_addCase(dpiTest_2827,
            "dpiLob_setFromBytes() with value NULL and valueLength zero");
    dpiTestSuite_addCase(dpiTest_2828,
            "dpiLob_setFromBytes() with value not NULL and valueLength "
            "non-zero");
    return dpiTestSuite_run();
}
