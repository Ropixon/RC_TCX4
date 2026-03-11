/*
 * KeyBoardProcess.h
 *
 *  Created on: 27. 6. 2021
 *      Author: developer
 */

#ifndef SRC_CORE_TASK_KEYBOARDPROCESS_H_
#define SRC_CORE_TASK_KEYBOARDPROCESS_H_

void KP_KeyBoardActivated(tDataQueue ReceiveData,tCoreGlobalData *glData,tStateCoreAutomat* StateAutomat);
void KP_ShortPress(uint32_t button,tCoreGlobalData *GlobalData);
void KP_LongPressPress(uint32_t button,tCoreGlobalData *GlobalData,tStateCoreAutomat* StateAutomat);
void KP_KeyPLUS(tCoreGlobalData *glData);
void KP_KeyMinus(tCoreGlobalData *glData);
void KP_KeyLeft(tCoreGlobalData *glData);
void KP_KeyRight(tCoreGlobalData *glData);
void KP_KeyBoardLight(bool shortPress,bool impulse,bool onlyLight, tCoreGlobalData *glData );
bool KP_KeyboardFallingEdge(uint32_t button,tCoreGlobalData *glData);
void KP_KeyboarRisingEdge(tCoreGlobalData *glData);
bool KP_ApplicablePressBOTTOMKEYS(uint32_t button, tCoreGlobalData *glData);
bool KP_ApplicablePressMINUS(tCoreGlobalData *glData);
bool KP_ApplicablePressPLUS(tCoreGlobalData *glData);
bool KP_ApplicablePressRIGHT(tCoreGlobalData *glData);
bool KP_ApplicablePressLEFT(tCoreGlobalData *glData);

#endif /* SRC_CORE_TASK_KEYBOARDPROCESS_H_ */
