/*
 This file is part of WME Lite.
 http://dead-code.org/redir.php?target=wmelite
 
 Copyright (c) 2011 Jan Nedoma
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#import "IOS_StoreKit.h"


@implementation StoreKitMgr

// C interface
void StoreKit_SetExternalData(void* data)
{
    [[StoreKitMgr sharedStoreKitMgr] setExternalData:data];
}

int StoreKit_IsStoreAvailable()
{
    if ([[StoreKitMgr sharedStoreKitMgr] storeAvailable]) return 1;
    else return 0;
}

void StoreKit_EnableEvents()
{
    [[StoreKitMgr sharedStoreKitMgr] enableEvents];
}

void StoreKit_DisableEvents()
{
    [[StoreKitMgr sharedStoreKitMgr] disableEvents];    
}

void StoreKit_ValidateProducts(const char* ids)
{
    [[StoreKitMgr sharedStoreKitMgr] validateProducts:ids];
}

void StoreKit_Purchase(const char* prodId)
{
    [[StoreKitMgr sharedStoreKitMgr] purchase:prodId];
}

void StoreKit_RestoreTransactions()
{
    [[StoreKitMgr sharedStoreKitMgr]restoreTransactions];
}

int StoreKit_FinishTransaction(const char* transId)
{
    if ([[StoreKitMgr sharedStoreKitMgr]finishTransaction:transId]) return 1;
    else return 0;
}



static StoreKitMgr* _sharedStoreKitMgr = nil;

+ (StoreKitMgr*)sharedStoreKitMgr
{
	@synchronized([StoreKitMgr class])
	{
		if (!_sharedStoreKitMgr)
			[[self alloc] init];
		
		return _sharedStoreKitMgr;
	}
	return nil;
}

+(id)alloc
{
	@synchronized([StoreKitMgr class])
	{
		NSAssert(_sharedStoreKitMgr == nil, @"Attempted to allocate a second instance of a singleton.");
		_sharedStoreKitMgr = [super alloc];
		return _sharedStoreKitMgr;
	}
	return nil;
}

-(id) init
{
    dictTransactions = [[NSMutableDictionary alloc] init];
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

-(void)setExternalData:(void*)data
{
    externalData = data;
}

-(BOOL)storeAvailable
{
    return [SKPaymentQueue canMakePayments];
}

-(void)enableEvents
{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

-(void)disableEvents
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

- (void)productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response
{
    StoreKit_ReceiveProductsStartCallback(externalData);
    
    NSArray* products = response.products;
	for (SKProduct* product in products)
	{
        StoreKit_AddValidProductCallback([product.productIdentifier UTF8String], [product.localizedTitle UTF8String], [product.localizedDescription UTF8String], [product.localizedPrice UTF8String], externalData);
    }
    
    NSArray* invalid = response.invalidProductIdentifiers;
	for (NSString* invalidId in invalid)
	{
        StoreKit_AddInvalidProductCallback([invalidId UTF8String], externalData);
    }
    
    StoreKit_ReceiveProductsEndCallback(externalData);
}

- (void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
{
    //[dictTransactions removeAllObjects];
    
    
    for (SKPaymentTransaction* transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
            case SKPaymentTransactionStateFailed:
            case SKPaymentTransactionStateRestored:
                [dictTransactions setObject:transaction forKey:transaction.transactionIdentifier];
                break;
            default:
                break;
        }
    }
    
    StoreKit_ReceiveTransactionsStartCallback(externalData);
    
    NSArray* keys = [dictTransactions allKeys];
	for (NSString* key in keys)
    {
        SKPaymentTransaction* transaction = [dictTransactions objectForKey:key];
        [self processTransaction:transaction];
    }
    
    StoreKit_ReceiveTransactionsEndCallback(externalData);
}

-(void)processTransaction:(SKPaymentTransaction*)transaction
{
    NSString* state = @"";
    
    switch (transaction.transactionState)
    {
        case SKPaymentTransactionStatePurchased:
            state = @"purchased";
            break;
        
        case SKPaymentTransactionStateFailed:
            if (transaction.error.code == SKErrorPaymentCancelled)
                state = @"cancelled";
            else
                state = @"failed";
            break;
        
        case SKPaymentTransactionStateRestored:
            state = @"restored";
            break;
    }
    
    
    
    StoreKit_AddTransactionCallback([transaction.transactionIdentifier UTF8String], [transaction.payment.productIdentifier UTF8String], [state UTF8String], externalData);
}

-(void)validateProducts:(const char*)ids
{
    NSString* strIdList = [NSString stringWithCString:ids encoding:NSUTF8StringEncoding];
    NSSet* idSet = [NSSet setWithArray:[strIdList componentsSeparatedByString:@";"]];
    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:idSet];
    productsRequest.delegate = self;
    [productsRequest start];
}

-(void)purchase:(const char*)prodId
{
    SKPayment* payment = [SKPayment paymentWithProductIdentifier:[NSString stringWithCString:prodId encoding:NSUTF8StringEncoding]];
    
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

-(void)restoreTransactions
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    StoreKit_RestoreFinishedCallback(externalData, 0);
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    StoreKit_RestoreFinishedCallback(externalData, 1);
}


-(BOOL)finishTransaction:(const char*)transId
{
    NSString* strId = [NSString stringWithCString:transId encoding:NSUTF8StringEncoding];
    
    SKPaymentTransaction* trans = [dictTransactions objectForKey:strId];
    if (trans != nil)
    {
        [[SKPaymentQueue defaultQueue] finishTransaction: trans];
        [dictTransactions removeObjectForKey:strId];
        return YES;
    }
    else return NO;
}

@end




@implementation SKProduct (LocalizedPrice)

- (NSString* )localizedPrice
{
    NSNumberFormatter* numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [numberFormatter setLocale:self.priceLocale];
    NSString* formattedString = [numberFormatter stringFromNumber:self.price];
    [numberFormatter release];
    return formattedString;
}

@end
