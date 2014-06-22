#ifndef __IOS_StoreKit_interface_H__
#define __IOS_StoreKit_interface_H__


#ifdef __cplusplus
extern "C" {
#endif
	void StoreKit_SetExternalData(void* data);
	int StoreKit_IsStoreAvailable();
    void StoreKit_EnableEvents();
    void StoreKit_DisableEvents();
    void StoreKit_ValidateProducts(const char* ids);
    void StoreKit_Purchase(const char* prodId);
    void StoreKit_RestoreTransactions();
    int StoreKit_FinishTransaction(const char* transId);
    
    // callbacks
    void StoreKit_AddValidProductCallback(const char* id, const char* name, const char* desc, const char* price, void* data);
    void StoreKit_AddInvalidProductCallback(const char* id, void* data);
    void StoreKit_ReceiveProductsStartCallback(void* data);
    void StoreKit_ReceiveProductsEndCallback(void* data);

    void StoreKit_AddTransactionCallback(const char* id, const char* productId, const char* state, void* data);
    void StoreKit_ReceiveTransactionsStartCallback(void* data);
    void StoreKit_ReceiveTransactionsEndCallback(void* data);

    void StoreKit_RestoreFinishedCallback(void* data, int error);
    
#ifdef __cplusplus
}
#endif


#endif // __IOS_StoreKit_interface_H__
