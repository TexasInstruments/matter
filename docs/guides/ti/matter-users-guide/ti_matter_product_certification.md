# Matter Product Certification

Matter certification is a necessary step to ensure interoperability with
existing Matter products. Each individual Matter product must go through
a process of certification in order to wear the Matter certified
interoperability logo.

> **__Attention__**
Matter product certification is not required if you are designing a
proprietary network setup using the Matter protocol. **However**, you
will not be awarded any IP rights or protections by not becoming a
CSA member and take a product through certification.

## Matter Stack Certification

TI's Matter solution is a Matter certified hardware solution.
Matter certification was done for the **lighting application**
specifically.

## Leveraging TI's Thread Certification for your Matter End product Certification

Customers using TI provided certified library for their Matter projects as described in the document `ti_openthread_library_usage.md` can leverage certification by similarity for Thread certification. Customers will need to use the TI certification ID for their product as listed in the table below.
Texas Instruments Thread Certification ID's

| Platform         | Configuration     | CID      |
|------------------|:-----------------:|:--------:|
| CC1354 or CC2674 | FTD               | 13A229   |
| CC1354 or CC2674 | MTD               | 13A230   |


## Matter v1.4 Certification

To certify your product using the above referenced Thread libs and this Matter release from TI, consider the following points below

1. Cluster Revision and Feature Map attributes for the clusters can be updated per release. Developers are recommended to look at the CSA Tag for that release (for v1.4 it's v1.4.0.0) and configure the values for the Matter version they are certifying for.
    * For example for v1.4: Cluster revision value for Network Commissioning cluster needs to be 2. This can be done by setting the value in the ZAP GUI and then generating the .matter file. Or by setting the values for some clusters where ZAP tool updates do not take effect in <connectedhomeip-repo-dir>/src/app/<relevant-cluster.c/h> file.

```