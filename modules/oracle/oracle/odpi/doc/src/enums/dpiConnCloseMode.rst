.. _dpiConnCloseMode:

ODPI-C Enumeration dpiConnCloseMode
-----------------------------------

This enumeration identifies the mode to use when closing connections to the
database.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiConnCloseMode
     enumeration. The second column displays the description of the
     dpiConnCloseMode enumeration value.

    * - Value
      - Description
    * - DPI_MODE_CONN_CLOSE_DEFAULT
      - Default value used when closing connections.
    * - DPI_MODE_CONN_CLOSE_DROP
      - Causes the session to be dropped from the session pool instead of
        simply returned to the pool for future use.
    * - DPI_MODE_CONN_CLOSE_RETAG
      - Causes the session to be tagged with the tag information given when the
        connection is closed. A value of NULL for the tag will cause the tag to
        be cleared.
