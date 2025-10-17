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
// test_2500_deq_options.c
//   Test suite for testing all the Dequeue Options related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2500()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2500(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiDeqOptions_release(deqOptions);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_2501()
//   Call each of the dpiDeqOptions public functions with the options parameter
// set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2501(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";
    dpiDeqNavigation navigation;
    uint32_t valueLength, time;
    dpiVisibility visibility;
    const char *value;
    dpiDeqMode mode;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getCondition(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getConsumerName(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getCorrelation(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getMode(NULL, &mode);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getMsgId(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getNavigation(NULL, &navigation);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getTransformation(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getVisibility(NULL, &visibility);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getWait(NULL, &time);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setCondition(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setConsumerName(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setCorrelation(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setDeliveryMode(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setMode(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setMsgId(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setNavigation(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setTransformation(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setVisibility(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setWait(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2502()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_setNavigation(); call
// dpiDeqOptions_getNavigation() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2502(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiDeqNavigation setValue = DPI_DEQ_NAV_FIRST_MSG, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getNavigation(deqOptions, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2503()
//   call dpiConn_newDeqOptions(); call dpiDeqOptions_setWait(); call
// dpiDeqOptions_getWait() and verify that the value returned matches
// the value that was set (no error)
//-----------------------------------------------------------------------------
int dpiTest_2503(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t setValue = 4, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getWait(deqOptions, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2504()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_setMode(); call
// dpiDeqOptions_getMode() and verify that the value returned matches the value
// that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2504(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiDeqMode setValue = DPI_MODE_DEQ_BROWSE, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setMode(deqOptions, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMode(deqOptions, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2505()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_setCorrelation(); call
// dpiDeqOptions_getCorrelation() and verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2505(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *setValue = "_", *getValue;
    dpiDeqOptions *deqOptions;
    uint32_t getValueLength;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setCorrelation(deqOptions, setValue,
            strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCorrelation(deqOptions, &getValue,
            &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2506()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setVisibility(),
// call dpiDeqOptions_getVisibility() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2506(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiVisibility visModes[] = {DPI_VISIBILITY_ON_COMMIT,
            DPI_VISIBILITY_IMMEDIATE, -1}, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; visModes[i] != -1; i++) {
        if (dpiDeqOptions_setVisibility(deqOptions, visModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiDeqOptions_getVisibility(deqOptions, &getValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, getValue, visModes[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2507()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setMsgId(),
// call dpiDeqOptions_getMsgId() and verify that the value returned matches
// the value that was set (no error)
//-----------------------------------------------------------------------------
int dpiTest_2507(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *setMsgId = "TEST_MSGID", *getMsgId;
    dpiDeqOptions *deqOptions;
    uint32_t getMsgIdLen;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMsgId(deqOptions, &getMsgId, &getMsgIdLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getMsgId, getMsgIdLen, NULL,
            0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setMsgId(deqOptions, setMsgId, strlen(setMsgId)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMsgId(deqOptions, &getMsgId, &getMsgIdLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getMsgId, getMsgIdLen,
            setMsgId, strlen(setMsgId)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2508()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setCondition(),
// call dpiDeqOptions_getCondition() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2508(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *setCond = "tab.priority between 2 and 4", *getCond;
    dpiDeqOptions *deqOptions;
    uint32_t getCondLen;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCondition(deqOptions, &getCond, &getCondLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getCond, getCondLen,
            NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setCondition(deqOptions, setCond, strlen(setCond)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCondition(deqOptions, &getCond, &getCondLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getCond, getCondLen,
            setCond, strlen(setCond)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2509()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setConsumerName(),
// call dpiDeqOptions_getConsumerName() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2509(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *setConsName = "TEST_CONSUMER!", *getConsName;
    dpiDeqOptions *deqOptions;
    uint32_t getConsNameLen;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getConsumerName(deqOptions, &getConsName,
            &getConsNameLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getConsName, getConsNameLen,
            NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setConsumerName(deqOptions, setConsName,
            strlen(setConsName)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getConsumerName(deqOptions, &getConsName,
            &getConsNameLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getConsName, getConsNameLen,
            setConsName, strlen(setConsName)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2500);
    dpiTestSuite_addCase(dpiTest_2500,
            "call dpiDeqOptions_release() twice");
    dpiTestSuite_addCase(dpiTest_2501,
            "call all dpiDeqOptions functions with options param as NULL");
    dpiTestSuite_addCase(dpiTest_2502,
            "verify Navigation is set as expected");
    dpiTestSuite_addCase(dpiTest_2503,
            "verify wait time is set as expected");
    dpiTestSuite_addCase(dpiTest_2504,
            "verify mode is set as expected");
    dpiTestSuite_addCase(dpiTest_2505,
            "verify correlation is set as expected");
    dpiTestSuite_addCase(dpiTest_2506,
            "verify visibility mode is set as expected");
    dpiTestSuite_addCase(dpiTest_2507,
            "verify message id is set as expected");
    dpiTestSuite_addCase(dpiTest_2508,
            "verify condition is set as expected");
    dpiTestSuite_addCase(dpiTest_2509,
            "verify consumer name is set as expected");
    return dpiTestSuite_run();
}
