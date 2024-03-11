// 0x6080604052348015600e575f80fd5b5060a58061001b5f395ff3fe6080604052348015600e575f80fd5b50600436106030575f3560e01c8063cdfead2e146034578063e026c017146045575b5f80fd5b6043603f3660046059565b5f55565b005b5f5460405190815260200160405180910390f35b5f602082840312156068575f80fd5b503591905056fea2646970667358221220ee718384a20a40f58b676e7f7ba7521f229a5f787acec88f60bea1052b0d150d64736f6c63430008140033

// 3 sections:
// 1. Contract Creation
// 2. Runtime
// 3. Metadata 

// 1. Contract Creation Code
// Free memory Pointer
PUSH1 0x80      // [0x80]
PUSH1 0x40      // [0x40, 0x80]
MSTORE          // []       Memory 0x40 -> 0x80

// If someone sent value with this call, jump to the 0x0E PC/JumpDest
CALLVALUE       // [msg.value]
DUP1            // [msg.value, msg.value]
ISZERO          // [msg.value == 0, msg.value]
PUSH1 0x0e      // [0x0E, msg.value == 0, msg.value]
JUMPI           // [msg.value]
PUSH0           // [0x00, msg.value]
DUP1            // [0x00, 0x00, msg.value]
REVERT          // [msg.value]

// This part can be deleted to save gas costs when deploying the contract
// Or add constructor() payable {} in the smart contract

// Jump dest if msg.value == 0
// Sticks the runtime code on chain
JUMPDEST        // [msg.value]
POP             // []
PUSH1 0xa5      // [0xa5]
DUP1            // [0xa5, 0xa5]
PUSH2 0x001b    // [0x001b, 0xa5, 0xa5]
PUSH0           // [0x00, 0x001b, 0xa5, 0xa5]
CODECOPY        // [0xa5]               Memory: [runtie code]
PUSH0           // [0x00, 0xa5]
RETURN          // []
INVALID         // []

// 2. Runtime Code
// Entry point of all calls
// free memory pointer
PUSH1 0x80
PUSH1 0x40
MSTORE          // Memory [0x40: 0x80]

CALLVALUE       // [msg.value]
DUP1            // [msg.value, msg.value]
ISZERO          // [msg.value == 0, msg.value]
PUSH1 0x0e      // [0x0E, msg.value == 0, msg.value]
JUMPI           // [msg.value]
// Jump to "continue"   if msg.value == 0

PUSH0           // [0x00, msg.value]
DUP1            // [0x00, 0x00, msg.value, msg.value]
REVERT          // [msg.value]

// Jump dest if msg.value == 0, start here!
// contiune
JUMPDEST        // [msg.value]
POP             // []
PUSH1 0x04      // [0x04]
CALLDATASIZE    // [calldata_size, 0x04]
LT              // [calldata_size < 0x04]
PUSH1 0x30      // [0x30, calldata_size < 0x04]
JUMPI           // []
// if calldata_size < 0x04 -> calldata_jump

// Function dispatching in solidity!
PUSH0           // [0]
CALLDATALOAD    // [32bytes of calldata]
PUSH1 0xe0      // [0xe0, 32bytes of calldata]
SHR             // [calldata[0:4]] // function_selector

// // Function dispatching for updateHorseNumber!
DUP1            // [function_selector, function_selector]
PUSH4 0xcdfead2e    // [0xcdfead2e, function_selector, function_selector] updateHorseNumber(uint256)
EQ              // [function_selector == 0xcdfead2e, function_selector]
PUSH1 0x34      // [0x34, function_selector == 0xcdfead2e, function_selector]
JUMPI           // [function_selector]
// if function_selector == 0xcdfead2e -> set_number_of_horses

// Function dispatching for readNumberOfHorses
DUP1            // [function_selector, function_selector]
PUSH4 0xe026c017    // [0xe026c017, function_selector, function_selector]
EQ              // [function_selector == 0xe026c017, function_selector]
PUSH1 0x45      // [0x45, function_selector == 0xe026c017, function_selector]
JUMPI           // [function_selector]
// if function_selector == 0xe026c017 -> get_number_of_horses

// calldata_jump
// Revert  Jumpdest
JUMPDEST        // []
PUSH0           // [0]
DUP1            // [0, 0]
REVERT          // []

// updateHorsesNumber jump dest 1
// Setup jumping program counters in the stack
JUMPDEST        // [function_selector]
PUSH1 0x43      // [0x43, function_selector]
PUSH1 0x3f      // [0x3f, 0x43, function_selector]
CALLDATASIZE    // [calldata_siz, 0x3f, 0x43, function_selector]
PUSH1 0x04      // [0x04, calldata_siz, 0x3f, 0x43, function_selector]
PUSH1 0x59      // [0x59, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
JUMP            // [0x04, calldata_siz, 0x3f, 0x43, function_selector]

// updateHorsesNumber jump dest 4
// We can finally run an sstore to save our value to storage:
// 1. Function dispath
// 2. Checked for msg.value
// 3. Checked that calldata is long enough
// 4. Received the number to use from the calldata

JUMPDEST        // [calldata (of numberToUpdate), 0x43, function_selector]
PUSH0           // [0, calldata (of numberToUpdate), 0x43, function_selector]
SSTORE          // [0x43, function_selector]
JUMP            // [function_selector]
//  jump to jump dest 5

// updateHorsesNumber jump dest 5
JUMPDEST        // [function_selector]
STOP            // [function_selector]

// readNumberOfHorses jump dest 1
// the only jump dest
JUMPDEST        // [function_selector]
PUSH0           // [0, function_selector]
SLOAD           // [numberHorses, function_selector]
PUSH1 0x40      // [0x40, numberHorses, function_selector]
MLOAD           // [0x80, numberHorses, function_selector] Memory [0x40: 0x80] (free memory pointer)
SWAP1           // [numberHorses, 0x80, function_selector]
DUP2            // [0x80, numberHorses, 0x80, function_selector]
MSTORE          // [0x80, function_selector]
PUSH1 0x20      // [0x20, 0x80, function_selector]
ADD             // [0xa0, function_selector]
PUSH1 0x40      // [0x40, 0xa0, function_selector]
MLOAD           // [0x80, 0xa0, function_selector]
DUP1            // [0x80, 0x80, 0xa0, function_selector]
SWAP2           // [0xa0, 0x80, 0x80, function_selector]
SUB             // [0xa0 - 0x80, 0x80, function_selector]
SWAP1           // [0x80, 0xa0 - 0x80, function_selector]
// Return a value of size 32 bytes, that's located at position 0x80 in memory
RETURN          // [function_selector]

// updateHorsesNumber jump dest 2
// Check to see if there is a vaule to update the horse number to
// 4 bytes for function selector, 32 bytes for horse number
JUMPDEST        // [0x04, calldata_siz, 0x3f, 0x43, function_selector]
PUSH0           // [0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
PUSH1 0x20      // [0x20, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
DUP3            // [0x04, 0x20, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
DUP5            // [calldata_siz, 0x04, 0x20, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
SUB             // [calldata_siz - 0x04, 0x20, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
SLT             // [calldata_siz - 0x04 < 0x20, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
ISZERO          // [more_calldata_than_selector?, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
PUSH1 0x68      // [0x68, more_calldata_than_selector?, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
JUMPI           // [0x68, more_calldata_than_selector?, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
// We are going to jump to jump dest 3 if there is more calldata than:
// function selector + 0x20

// Revert if there isn't enough calldata!
PUSH0           // [0, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
DUP1            // [0, 0, 0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
REVERT          // [0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]

// updateHorsesNumber jump dest 3
// Grab the calldata for updating the horse number
// Delete some stuff in the stack
JUMPDEST        // [0, 0x04, calldata_siz, 0x3f, 0x43, function_selector]
POP             // [0x04, calldata_siz, 0x3f, 0x43, function_selector]
// Ignore the function selector, and just grab the data
// 0xcdfead2e 0000000000000000000000000000000000000000000000000000000000000007
CALLDATALOAD    // [calldata, calldata_siz, 0x3f, 0x43, function_selector]
SWAP2           // [0x3f, calldata_siz, calldata (of numberToUpdate), 0x3f, 0x43, function_selector]
SWAP1           // [calldata_siz, 0x3f, calldata (of numberToUpdate), 0x3f, 0x43, function_selector]
POP             // [0x3f, calldata (of numberToUpdate), 0x3f, 0x43, function_selector]
JUMP            // [0x3f, calldata (of numberToUpdate), 0x3f, 0x43, function_selector]
// jump to jump dest 4

// 3. Metadata
INVALID
LOG2
PUSH5 0x6970667358
INVALID
SLT
SHA3
INVALID
PUSH18 0x8384a20a40f58b676e7f7ba7521f229a5f78
PUSH27 0xcec88f60bea1052b0d150d64736f6c63430008140033