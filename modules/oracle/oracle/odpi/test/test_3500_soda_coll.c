//-----------------------------------------------------------------------------
// Copyright (c) 2018, 2025, Oracle and/or its affiliates.
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
// test_3500_soda_coll.c
//   Test suite for testing SODA Collection functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"


//-----------------------------------------------------------------------------
// dpiTest__verifyContent()
//   Fetch the content of the document and verify it matches the expected
// content.
//-----------------------------------------------------------------------------
int dpiTest__verifyContent(dpiTestCase *testCase, dpiSodaColl *coll,
        dpiSodaOperOptions *options, const char *expectedContent)
{
    const char *content, *encoding;
    uint32_t contentLength;
    dpiSodaDoc *doc;

    if (dpiSodaColl_findOne(coll, options, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getContent(doc, &content, &contentLength, &encoding) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, content, contentLength,
            expectedContent, strlen(expectedContent)) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__countDocuments()
//   Count the documents found in the collection using a document cursor and
// iterating over the cursor.
//-----------------------------------------------------------------------------
int dpiTest__countDocuments(dpiTestCase *testCase, dpiSodaColl *coll,
        dpiSodaOperOptions *options, uint64_t expectedCount)
{
    dpiSodaDocCursor *cursor;
    dpiSodaDoc *doc;
    uint64_t pos;

    // create cursor
    if (dpiSodaColl_find(coll, options, DPI_SODA_FLAGS_DEFAULT, &cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // iterate over all documents and ensure count matches
    for (pos = 0; pos < expectedCount; pos++) {
        if (dpiSodaDocCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!doc)
            return dpiTestCase_setFailed(testCase,
                    "too few documents found in the database");
        if (dpiSodaDoc_release(doc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // no further document should be found
    if (dpiSodaDocCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (doc)
        return dpiTestCase_setFailed(testCase,
                "too many documents found in the database");
    if (dpiSodaDocCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__insertDoc()
//   Create a document and insert it into the collection.
//-----------------------------------------------------------------------------
int dpiTest__insertDoc(dpiTestCase *testCase, dpiSodaDb *db,
        const char *content, dpiSodaColl *coll, dpiSodaDoc **insertedDoc)
{
    dpiSodaDoc *doc;

    if (dpiSodaDb_createDocument(db, NULL, 0, content, strlen(content), NULL,
            0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_insertOne(coll, doc, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__saveDoc()
//   Create a document and save it into the collection.
//-----------------------------------------------------------------------------
int dpiTest__saveDoc(dpiTestCase *testCase, dpiSodaDb *db, const char *key,
        uint32_t keyLength, const char *content, dpiSodaColl *coll,
        dpiSodaDoc **savedDoc)
{
    dpiSodaDoc *doc;

    if (dpiSodaDb_createDocument(db, key, keyLength, content, strlen(content),
            NULL, 0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_save(coll, doc, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            savedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__insertManyDocs()
//   Create a set of documents and insert it into the collection using
// dpiSodaColl_insertMany(). Verifies the content if required.
//-----------------------------------------------------------------------------
int dpiTest__insertManyDocs(dpiTestCase *testCase, dpiSodaDb *db,
        dpiSodaColl *coll, uint32_t numDocs, const char **content,
        dpiSodaDoc **insertedDocs)
{
    uint64_t origDocCount, docCount;
    dpiSodaOperOptions options;
    dpiContext *context;
    dpiSodaDoc **docs;
    uint32_t i;

    // get original document count
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &origDocCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create documents for each of the contents provided
    docs = malloc(numDocs * sizeof(dpiSodaDoc*));
    if (!docs)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    for (i = 0; i < numDocs; i++) {
        if (dpiSodaDb_createDocument(db, NULL, 0, content[i],
                strlen(content[i]), NULL, 0, DPI_SODA_FLAGS_DEFAULT,
                &docs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // perform bulk insert
    if (dpiSodaColl_insertMany(coll, numDocs, docs,
            DPI_SODA_FLAGS_ATOMIC_COMMIT, insertedDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // clean up created documents
    for (i = 0; i < numDocs; i++)
        dpiSodaDoc_release(docs[i]);
    free(docs);

    // get document count and verify it matches expectations
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &docCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, docCount,
            origDocCount + numDocs) < 0)
        return DPI_FAILURE;

    // if result documents are returned, verify that the contents stored match
    // what was inserted
    if (insertedDocs) {

        // initialize operation options
        dpiTestSuite_getContext(&context);
        if (dpiContext_initSodaOperOptions(context, &options) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // verify content for each result document returned
        for (i = 0; i < numDocs; i++) {
            if (dpiSodaDoc_getKey(insertedDocs[i], &options.key,
                    &options.keyLength) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (dpiTest__verifyContent(testCase, coll, &options,
                    content[i]) < 0)
                return DPI_FAILURE;
        }

    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3500()
//   Call all public collection functions with NULL handle and verify that the
// correct error is returned in each case.
//-----------------------------------------------------------------------------
int dpiTest_3500(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    dpiSodaColl_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_createIndex(NULL, NULL, 0, DPI_SODA_FLAGS_DEFAULT);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_drop(NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_dropIndex(NULL, NULL, 0, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_find(NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_findOne(NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_getDataGuide(NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_getDocCount(NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_getMetadata(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_getName(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_insertOne(NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_insertMany(NULL, 0, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_remove(NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_replaceOne(NULL, NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL,
            NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_truncate(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaColl_save(NULL, NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3501()
//   Verify dpiSodaColl_addRef() works as expected.
//-----------------------------------------------------------------------------
int dpiTest_3501(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *collName = "ODPIC_COLL_2601";
    dpiSodaColl *coll;
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_addRef(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_release(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    dpiSodaColl_release(coll);
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3502()
//   Create a document, replace it and verify that it was replaced properly.
//-----------------------------------------------------------------------------
int dpiTest_3502(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *replaceContent = "{\"test\":\"2602 replaced\"}";
    const char *content = "{\"test\":\"2602 original\"}";
    const char *collName = "ODPIC_COLL_2602";
    dpiSodaDoc *doc, *insertedDoc, *replacedDoc;
    dpiSodaOperOptions options;
    dpiContext *context;
    dpiSodaColl *coll;
    dpiSodaDb *db;
    int replaced;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert new SODA document
    if (dpiTest__insertDoc(testCase, db, content, coll, &insertedDoc) < 0)
        return DPI_FAILURE;

    // initialize options to find document by key
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getKey(insertedDoc, &options.key, &options.keyLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify document was stored properly
    if (dpiTest__verifyContent(testCase, coll, &options, content) < 0)
            return DPI_FAILURE;

    // replace document with new content
    if (dpiSodaDb_createDocument(db, NULL, 0, replaceContent,
            strlen(replaceContent), NULL, 0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_replaceOne(coll, &options, doc,
            DPI_SODA_FLAGS_ATOMIC_COMMIT, &replaced, &replacedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, replaced, 1) < 0)
        return DPI_FAILURE;

    // verify document was replaced properly
    if (dpiTest__verifyContent(testCase, coll, &options, replaceContent) < 0)
            return DPI_FAILURE;

    // cleanup
    if (dpiSodaDoc_release(insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(replacedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3503()
//   Create a document, remove it and verify that it was removed properly.
//-----------------------------------------------------------------------------
int dpiTest_3503(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *content = "{\"test\":\"2603 content\"}";
    const char *collName = "ODPIC_COLL_2603";
    dpiSodaOperOptions options;
    dpiSodaDoc *insertedDoc;
    dpiContext *context;
    dpiSodaColl *coll;
    uint64_t numDocs;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert new SODA document
    if (dpiTest__insertDoc(testCase, db, content, coll, &insertedDoc) < 0)
        return DPI_FAILURE;

    // initialize options to find document by key
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getKey(insertedDoc, &options.key, &options.keyLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // remove document
    if (dpiSodaColl_remove(coll, &options, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 1) < 0)
        return DPI_FAILURE;

    // attempt to remove document a second time
    if (dpiSodaColl_remove(coll, &options, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiSodaDoc_release(insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3504()
//   Create a collection and drop it, then attempt to drop it a second time.
// It should not fail but it should indicate that the drop wasn't needed.
//-----------------------------------------------------------------------------
int dpiTest_3504(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *name = "ODPIC_COLL_2604";
    dpiSodaColl *coll;
    int isDropped;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, name, strlen(name), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // drop collection for the first time
    if (dpiSodaColl_drop(coll, DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isDropped, 1) < 0)
        return DPI_FAILURE;

    // drop collection for the second time
    if (dpiSodaColl_drop(coll, DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isDropped, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiSodaColl_release(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3505()
//   Create an index and drop it, then attempt to drop it a second time.
// It should not fail but it should indicate that the drop wasn't needed.
//-----------------------------------------------------------------------------
int dpiTest_3505(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *indexSpec = "{'name': 'ODPIC_COLL_2605_IX_1'}";
    const char *indexName = "ODPIC_COLL_2605_IX_1";
    const char *collName = "ODPIC_COLL_2605";
    dpiSodaColl *coll;
    dpiSodaDb *db;
    int isDropped;

    // get SODA database and drop all existing collections
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_dropAllSodaColls(testCase, db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
                DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create index
    if (dpiSodaColl_createIndex(coll, indexSpec, strlen(indexSpec),
                DPI_SODA_FLAGS_DEFAULT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // drop index for the first time
    if (dpiSodaColl_dropIndex(coll, indexName, strlen(indexName),
                DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isDropped, 1) < 0)
        return DPI_FAILURE;

    // drop index for the second time
    if (dpiSodaColl_dropIndex(coll, indexName, strlen(indexName),
                DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isDropped, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3506()
//   Create a collection and populate it with a number of documents, then
// perform counts to verify the the right number of documents is being
// returned.
//-----------------------------------------------------------------------------
int dpiTest_3506(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *content = "{\"test\" : \"2606 content\"}";
    const char *collName = "ODPIC_COLL_2606";
    dpiSodaOperOptions options;
    dpiSodaDoc *insertedDoc;
    dpiContext *context;
    dpiSodaColl *coll;
    uint64_t numDocs;
    dpiSodaDb *db;

    // get SODA database and drop all existing collections
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_dropAllSodaColls(testCase, db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // count of empty collection should be zero
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 0) < 0)
        return DPI_FAILURE;

    // insert documents and retain key of one of them for later count
    if (dpiTest__insertDoc(testCase, db, content, coll, &insertedDoc) < 0)
        return DPI_FAILURE;
    if (dpiTest__insertDoc(testCase, db, content, coll, NULL) < 0)
        return DPI_FAILURE;

    // count should now be 2
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 2) < 0)
        return DPI_FAILURE;

    // perform count with key specified in options
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getKey(insertedDoc, &options.key, &options.keyLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_getDocCount(coll, &options, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 1) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiSodaDoc_release(insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3507()
//   Create a collection, get the name of the collection and verify it is the
// same.
//-----------------------------------------------------------------------------
int dpiTest_3507(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *collName = "ODPIC_COLL_2607";
    uint32_t valueLength;
    const char *value;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get the name of the collection and verify
    if (dpiSodaColl_getName(coll, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, value, valueLength,
            collName, strlen(collName)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3508()
//   Create a collection and get metadata of it. Try to create collection with
// existing name and matching metadata and verify it is working. Also create
// collection with existing name and non matching metadata and verify it does
// not work.
//-----------------------------------------------------------------------------
int dpiTest_3508(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *collName1 = "ODPIC_COLL_2608_A";
    const char *collName2 = "ODPIC_COLL_2608_B";
    dpiSodaColl *coll1, *coll2;
    uint32_t metadataLength;
    const char *metadata;
    dpiSodaDb *db;

    // get SODA database and drop all existing collections
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_dropAllSodaColls(testCase, db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName1, strlen(collName1), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get metadata of collection and attempt to create with the same value
    if (dpiSodaColl_getMetadata(coll1, &metadata, &metadataLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_createCollection(db, collName1, strlen(collName1), metadata,
            metadataLength, DPI_SODA_FLAGS_DEFAULT, &coll2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_release(coll2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create SODA collection with different name
    if (dpiSodaDb_createCollection(db, collName2, strlen(collName2), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // now attempt to create the second SODA collection using the metadata of
    // the first one; this should fail
    dpiSodaDb_createCollection(db, collName2, strlen(collName2), metadata,
            metadataLength, DPI_SODA_FLAGS_DEFAULT, &coll2);
    if (dpiTestCase_expectError(testCase, "ORA-40669:") < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_cleanupSodaColl(testCase, coll2) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3509()
//   Insert set of documents into collection, call dpiSodaColl_find() without
// specifying options and verify it finds all the documents in the collection.
// call dpiSodaColl_find() by specifying options and verify documents that
// match with options are only found.
//-----------------------------------------------------------------------------
int dpiTest_3509(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *content = "{\"test\":\"2609\"}";
    const char *collName = "ODPIC_COLL_2609";
    dpiSodaDoc **temp, *insertedDoc;
    dpiSodaOperOptions options;
    uint32_t i, numDocs = 5;
    dpiContext *context;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // get SODA database and drop all existing collections
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_dropAllSodaColls(testCase, db) < 0)
        return DPI_FAILURE;

    // create SODA collection and verify that no documents exist in it
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__countDocuments(testCase, coll, NULL, 0) < 0)
        return DPI_FAILURE;

    // populate collection with some documents; retain only the first document
    // in order to perform a search by key
    for (i = 0; i < numDocs; i++) {
        temp = (i == 0) ? &insertedDoc : NULL;
        if (dpiTest__insertDoc(testCase, db, content, coll, temp) < 0)
            return DPI_FAILURE;
    }
    if (dpiTest__countDocuments(testCase, coll, NULL, numDocs) < 0)
        return DPI_FAILURE;

    // perform a search by key and verify only that document is returned
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getKey(insertedDoc, &options.key, &options.keyLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__countDocuments(testCase, coll, &options, 1) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiSodaDoc_release(insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3510()
//   Try to insert an invalid JSON value into a SODA collection and verify that
// it throws an error.
//-----------------------------------------------------------------------------
int dpiTest_3510(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedErrors[] = { "ORA-02290:", "ORA-40479:", "ORA-40780:",
            NULL };
    const char *content = "{\"test : 2610 content\"}";
    const char *collName = "ODPIC_COLL_2610";
    dpiSodaColl *coll;
    dpiSodaDoc *doc;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert new SODA document and verify it fails
    if (dpiSodaDb_createDocument(db, NULL, 0, content, strlen(content), NULL,
            0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaColl_insertOne(coll, doc, DPI_SODA_FLAGS_ATOMIC_COMMIT, NULL);
    if (dpiTestCase_expectAnyError(testCase, expectedErrors) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3511()
//   Insert a set of documents into the collection. Verify that finding,
// counting and removing documents works as expected when specifying a set of
// keys.
//-----------------------------------------------------------------------------
int dpiTest_3511(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t i, keyLengths[3], numDocs = 5, numKeys = 3;
    const char *content = "{\"test\":\"2611\"}";
    const char *collName = "ODPIC_COLL_2611";
    dpiSodaDoc *doc, *retainedDocs[3];
    dpiSodaOperOptions options;
    const char *keys[3];
    dpiContext *context;
    dpiSodaColl *coll;
    uint64_t count;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection and remove any documents that exist in it
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_remove(coll, NULL, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate collection with five documents; retain the keys of three of
    // these documents for further testing
    for (i = 0; i < numDocs; i++) {
        if (dpiTest__insertDoc(testCase, db, content, coll, &doc) < 0)
            return DPI_FAILURE;
        if (i % 2 == 0) {
            retainedDocs[i >> 1] = doc;
            if (dpiSodaDoc_getKey(doc, &keys[i >> 1], &keyLengths[i >> 1]) < 0)
                return dpiTestCase_setFailedFromError(testCase);
        } else if (dpiSodaDoc_release(doc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // verify that all documents were inserted correctly
    if (dpiTest__countDocuments(testCase, coll, NULL, numDocs) < 0)
        return DPI_FAILURE;

    // populate SODA operation options structure with keys of retained
    // documents
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    options.numKeys = numKeys;
    options.keys = keys;
    options.keyLengths = keyLengths;

    // verify that iterating with keys works as expected
    if (dpiTest__countDocuments(testCase, coll, &options, numKeys) < 0)
        return DPI_FAILURE;

    // verify that counting with keys works as expected
    if (dpiSodaColl_getDocCount(coll, &options, DPI_SODA_FLAGS_DEFAULT,
            &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count, numKeys) < 0)
        return DPI_FAILURE;

    // remove documents and verify
    if (dpiSodaColl_remove(coll, &options, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count, numKeys) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < numKeys; i++) {
        if (dpiSodaDoc_release(retainedDocs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3512()
//   Insert a set of documents into a collection. Specify values for skip and
// limit in the options structure and verify dpiSodaColl_find() works as
// expected.
//-----------------------------------------------------------------------------
int dpiTest_3512(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *content = "{\"test\":\"2611\"}";
    const char *collName = "ODPIC_COLL_2611";
    dpiSodaOperOptions options;
    uint32_t i, numDocs = 50;
    dpiContext *context;
    dpiSodaColl *coll;
    uint64_t count;
    dpiSodaDb *db;

    // get SODA database and drop all existing collections
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection and remove any documents that exist in it
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_remove(coll, NULL, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate collection with some documents and verify they were all
    // inserted
    for (i = 0; i < numDocs; i++) {
        if (dpiTest__insertDoc(testCase, db, content, coll, NULL) < 0)
            return DPI_FAILURE;
    }
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count, numDocs) < 0)
        return DPI_FAILURE;

    // pass a variety of skip and limit values and verify iteration succeeds
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    options.skip = 15;
    if (dpiTest__countDocuments(testCase, coll, &options,
            numDocs - options.skip) < 0)
        return DPI_FAILURE;
    options.skip = numDocs;
    if (dpiTest__countDocuments(testCase, coll, &options, 0) < 0)
        return DPI_FAILURE;
    options.skip = 0;
    options.limit = 20;
    if (dpiTest__countDocuments(testCase, coll, &options, options.limit) < 0)
        return DPI_FAILURE;
    options.skip = 10;
    options.limit = 5;
    if (dpiTest__countDocuments(testCase, coll, &options, options.limit) < 0)
        return DPI_FAILURE;
    options.skip = 40;
    options.limit = 20;
    if (dpiTest__countDocuments(testCase, coll, &options,
            numDocs - options.skip) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3513()
//   Create documents with large number of bytes, replace them, remove them
// and verify they are working as expected.
//-----------------------------------------------------------------------------
int dpiTest_3513(dpiTestCase *testCase, dpiTestParams *params)
{
    uint64_t numBytes[5] = {100000, 200000, 400000, 800000, 1048576};
    uint64_t numRemoved, numDocs = 5, i, j, lower = 49, upper = 90;
    const char *initStr = "{\"test\":\"", *termStr = "\"}";
    const char *collName = "ODPIC_COLL_2613";
    dpiSodaDoc *doc, *insertedDoc, *replacedDoc;
    uint64_t initStrLength = strlen(initStr);
    uint64_t termStrLength = strlen(termStr);
    char *content, *replaceContent;
    dpiSodaOperOptions options;
    dpiContext *context;
    dpiSodaColl *coll;
    dpiSodaDb *db;
    int replaced;

    // initialize content
    content = malloc(numBytes[numDocs - 1] + 1);
    replaceContent = malloc(numBytes[numDocs - 1] + 1);
    if (!content || !replaceContent)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    strcpy(content, initStr);
    strcpy(replaceContent, initStr);

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create documents of varying sizes with random content and ensure that
    // documents inserted or replaced have the content assigned to them (by
    // fetching the document after the operation has been completed)
    dpiTestSuite_getContext(&context);
    for (i = 0; i < numDocs; i++) {

        // create buffer with random data
        for (j = initStrLength; j < numBytes[i] - termStrLength; j++) {
            content[j] = (rand() % (upper - lower + 1)) + lower;
            replaceContent[j] = (rand() % (upper - lower + 1)) + lower;
        }
        strcpy(&content[numBytes[i] - termStrLength], termStr);
        strcpy(&replaceContent[numBytes[i] - termStrLength], termStr);

        // insert new SODA document
        if (dpiTest__insertDoc(testCase, db, content, coll, &insertedDoc) < 0)
            return DPI_FAILURE;

        // initialize options to find document by key
        if (dpiContext_initSodaOperOptions(context, &options) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiSodaDoc_getKey(insertedDoc, &options.key,
                &options.keyLength) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // verify document was stored properly
        if (dpiTest__verifyContent(testCase, coll, &options, content) < 0)
            return DPI_FAILURE;

        // replace document with new content
        if (dpiSodaDb_createDocument(db, NULL, 0, replaceContent,
                strlen(replaceContent), NULL, 0, DPI_SODA_FLAGS_DEFAULT,
                &doc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiSodaColl_replaceOne(coll, &options, doc,
                DPI_SODA_FLAGS_ATOMIC_COMMIT, &replaced, &replacedDoc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiSodaDoc_release(doc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, replaced, 1) < 0)
            return DPI_FAILURE;

        // verify document was replaced properly
        if (dpiTest__verifyContent(testCase, coll, &options,
                replaceContent) < 0)
            return DPI_FAILURE;

        // remove document
        if (dpiSodaColl_remove(coll, &options, DPI_SODA_FLAGS_ATOMIC_COMMIT,
                &numRemoved) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, numRemoved, 1) < 0)
            return DPI_FAILURE;

        // cleanup
        if (dpiSodaDoc_release(insertedDoc) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiSodaDoc_release(replacedDoc) < 0)
            return dpiTestCase_setFailedFromError(testCase);

    }

    // cleanup
    free(content);
    free(replaceContent);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3514()
//   Verify dpiSodaColl_insertMany() works as expected. Insert a set of docs
// and perform counts to verify the right number of documents is being
// returned.
//-----------------------------------------------------------------------------
int dpiTest_3514(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *contents[4] = {
        "{\"test1\":\"2614 content1\"}",
        "{\"test2\":\"2614 content2\"}",
        "{\"test3\":\"2614 content3\"}",
        "{\"test4\":\"2614 content4\"}"
    };
    const char *collName = "ODPIC_COLL_2614";
    dpiSodaDoc **insertedDocs;
    uint32_t numDocs = 4;
    uint64_t docCount;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // get SODA database (Oracle Client 18.5 required for bulk insert)
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 18, 5) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // remove any documents that may already exist due to previous failures
    if (dpiSodaColl_remove(coll, NULL, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &docCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert documents and verify contents
    insertedDocs = malloc(numDocs * sizeof(dpiSodaDoc*));
    if (!insertedDocs)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    if (dpiTest__insertManyDocs(testCase, db, coll, numDocs, contents,
            insertedDocs) < 0)
        return DPI_FAILURE;
    free(insertedDocs);

    // insert documents a second time without verifying contents
    if (dpiTest__insertManyDocs(testCase, db, coll, numDocs, contents,
            NULL) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3515()
//   Try to insert invalid JSON values into a SODA collection using
// dpiSodaColl_insertMany() and verify that it throws an error.
//-----------------------------------------------------------------------------
int dpiTest_3515(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedErrors[] = { "ORA-02290:", "ORA-40479:", "ORA-40780:",
            NULL };
    const char *contents[5] = {
        "{\"test1\" : \"2615 content1\"}",
        "{\"test2\" : \"2615 content2\"}",
        "{\"test3\" : \"2615 content3\"}",
        "{\"test4 : 2615 content4\"}",
        "{\"test5\" : \"2615 content5\"}",
    };
    const char *collName = "ODPIC_COLL_2615";
    uint32_t i, numDocs = 5;
    dpiErrorInfo errorInfo;
    dpiSodaColl *coll;
    dpiSodaDoc **docs;
    dpiSodaDb *db;

    // get SODA database (Oracle Client 18.5 required for bulk insert)
    // get SODA database
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 18, 5) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert SODA documents and verify it fails
    docs = malloc(numDocs * sizeof(dpiSodaDoc*));
    if (!docs)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    for (i = 0; i < numDocs; i++) {
        if (dpiSodaDb_createDocument(db, NULL, 0, contents[i],
                strlen(contents[i]), NULL, 0, DPI_SODA_FLAGS_DEFAULT,
                &docs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    dpiSodaColl_insertMany(coll, numDocs, docs, DPI_SODA_FLAGS_ATOMIC_COMMIT,
        NULL);
    if (dpiTestCase_expectAnyError(testCase, expectedErrors) < 0)
        return DPI_FAILURE;

    // verify offset is accurate
    dpiTestSuite_getErrorInfo(&errorInfo);
    if (dpiTestCase_expectIntEqual(testCase, errorInfo.offset, 3) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < numDocs; i++) {
        if (dpiSodaDoc_release(docs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    free(docs);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3516()
//   Insert a set of documents and perform a count to verify the right number
// of documents exists. After that, call dpiSodaColl_truncate() and verify that
// all documents have been removed.
//-----------------------------------------------------------------------------
int dpiTest_3516(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *contents[4] = {
        "{\"test1\":\"2616 content1\"}",
        "{\"test2\":\"2616 content2\"}",
        "{\"test3\":\"2616 content3\"}",
        "{\"test4\":\"2616 content4\"}"
    };
    const char *collName = "ODPIC_COLL_2616";
    uint64_t numDocs = 4;
    uint64_t docCount;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // Oracle Client 20.0 required for dpiSodaColl_truncate()
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 20, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // remove any documents that may already exist due to previous failures
    if (dpiSodaColl_remove(coll, NULL, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &docCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert documents
    if (dpiTest__insertManyDocs(testCase, db, coll, numDocs, contents,
            NULL) < 0)
        return DPI_FAILURE;

    // count should be 4
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 4) < 0)
        return DPI_FAILURE;

    // truncate
    if (dpiSodaColl_truncate(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // count should now be 0
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3517()
//   Create a collection and use dpiSodaColl_save() to first create and then
// update documents using client-assigned keys. Verify that the right number of
// documents exists after each operation.
//-----------------------------------------------------------------------------
int dpiTest_3517(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *metadata = "{"
        "\"keyColumn\":"
        "{"
            "\"name\": \"ID\","
            "\"sqlType\": \"NUMBER\","
            "\"assignmentMethod\": \"CLIENT\""
        "}"
    "}";
    const char *collName = "ODPIC_COLL_2617", *key1 = "1", *key2 = "2";
    const char *content = "{\"test\" : \"2617 content\"}";
    dpiSodaColl *coll;
    uint64_t numDocs;
    dpiSodaDb *db;

    // Oracle Client 19.9 required for dpiSodaColl_save()
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 19, 9) < 0)
        return DPI_FAILURE;

    // get SODA database and drop all existing collections
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_dropAllSodaColls(testCase, db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), metadata,
            strlen(metadata), DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // save document
    if (dpiTest__saveDoc(testCase, db, key1, strlen(key1), content, coll,
            NULL) < 0)
        return DPI_FAILURE;

    // count should now be 1
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 1) < 0)
        return DPI_FAILURE;

    // save document again
    if (dpiTest__saveDoc(testCase, db, key1, strlen(key1), content, coll,
            NULL) < 0)
        return DPI_FAILURE;

    // count should still be 1
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 1) < 0)
        return DPI_FAILURE;

    // save another document with a different key
    if (dpiTest__saveDoc(testCase, db, key2, strlen(key2), content, coll,
            NULL) < 0)
        return DPI_FAILURE;

    // count should now be 2
    if (dpiSodaColl_getDocCount(coll, NULL, DPI_SODA_FLAGS_DEFAULT,
            &numDocs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numDocs, 2) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3518()
//   Verify fetchArraySize works as expected. Insert a set of docs and perform
// counts to verify the right number of documents is being returned.
//-----------------------------------------------------------------------------
int dpiTest_3518(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *collName = "ODPIC_COLL_2618";
    dpiSodaOperOptions options;
    uint32_t numDocs = 40, i;
    dpiContext *context;
    uint64_t docCount;
    dpiSodaColl *coll;
    char buffer[100];
    dpiSodaDb *db;

    // get SODA database (Oracle Client 19.5 required for
    // dpiSodaOperOptions.fetchArraySize)
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 19, 5) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // remove any documents that may already exist due to previous failures
    if (dpiSodaColl_remove(coll, NULL, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &docCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate collection with some documents
    for (i = 0; i < numDocs; i++) {
        sprintf(buffer, "{\"test1\":\"2618 content%d\"}", i+1);
        if (dpiTest__insertDoc(testCase, db, buffer, coll, NULL) < 0)
            return DPI_FAILURE;
    }

    // set fetchArraySize
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    options.fetchArraySize = numDocs - 10;
    if (dpiTest__countDocuments(testCase, coll, &options, numDocs) < 0)
        return DPI_FAILURE;
    options.fetchArraySize = numDocs + 10;
    if (dpiTest__countDocuments(testCase, coll, &options, numDocs) < 0)
        return DPI_FAILURE;
    options.fetchArraySize = numDocs;
    if (dpiTest__countDocuments(testCase, coll, &options, numDocs) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3500);
    dpiTestSuite_addCase(dpiTest_3500,
            "call SODA collection functions with NULL handle");
    dpiTestSuite_addCase(dpiTest_3501,
            "dpiSodaColl_addRef() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3502,
            "dpiSodaColl_replaceOne() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3503,
            "dpiSodaColl_remove() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3504,
            "dpiSodaColl_drop() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3505,
            "dpiSodaColl_dropIndex() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3506,
            "dpiSodaColl_getDocCount() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3507,
            "dpiSodaColl_getName() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3508,
            "dpiSodaColl_getMetadata() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3509,
            "dpiSodaColl_find() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3510,
            "dpiSodaColl_insertOne() with invalid JSON");
    dpiTestSuite_addCase(dpiTest_3511,
            "verify find, getDocCount, remove with multiple keys");
    dpiTestSuite_addCase(dpiTest_3512,
            "dpiSodaColl_find() with skip and limit");
    dpiTestSuite_addCase(dpiTest_3513,
            "verify documents with large data");
    dpiTestSuite_addCase(dpiTest_3514,
            "dpiSodaColl_insertMany() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3515,
            "dpiSodaColl_insertMany() with invalid JSON");
    dpiTestSuite_addCase(dpiTest_3516,
            "dpiSodaColl_truncate() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3517,
            "dpiSodaColl_save() with valid parameters");
    dpiTestSuite_addCase(dpiTest_3518,
            "fetch documents with different fetchArraySize options");
    return dpiTestSuite_run();
}
