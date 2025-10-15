#include <Keyboard.h>
#include <Mouse.h>
 
// ----------------------
// PINの定義
// ----------------------
const int buttonPins[] = {2,3,4,5,6,7,8,9,10,11};
// 2:A, 3:S, 4:D, 5:K, 6:L, 7:;, 8:Space, 9:Space, 10:ESC, 11:Enter
 
const int numButtons = sizeof(buttonPins)/sizeof(buttonPins[0]);
 
// LED PIN
const int ledPinMap1 = 12; // keyMap1 状態表示
const int ledPinMap2 = 13; // keyMap2 状態表示
 
// ----------------------
// KeyMap 定義
// ----------------------
char keyMap1[] = {'a','s','d','k','l',';',' ',' ','\x1B','\n'}; // 音ゲー用
char keyMap2[] = {'q','e','r',0,0,0,' ',' ','\x1B','\n'};       // RPG用
 
// ----------------------
// 変換コマンド
// ----------------------
const int sequencePins[] = {2,2,3,3,4,4};
int seqIndex = 0;
 
// ----------------------
// Modifier PIN
// ----------------------
const int modifierPins[] = {10,11}; // ESC + Enter
 
// ----------------------
// ディバウンス / 繰り返し入力
// ----------------------
const unsigned long debounceDelay = 30;   // ボタンのバウンス削除
const unsigned long repeatDelay = 200;    // 反復入力速度(ms)
bool keyStates[10] = {0};
unsigned long lastDebounceTimes[10] = {0};
unsigned long lastRepeatTimes[10] = {0};
 
// ----------------------
// 現在のKeyMap状態
// ----------------------
bool useAltMap = false;
 
void setup(){
  for(int i=0;i<numButtons;i++){
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(ledPinMap1, OUTPUT);
  pinMode(ledPinMap2, OUTPUT);
  Keyboard.begin();
  Mouse.begin();
}
 
void loop(){
  // ----------------------
  // LED表示
  // ----------------------
  digitalWrite(ledPinMap1, useAltMap ? LOW : HIGH); // keyMap1 選択じにON
  digitalWrite(ledPinMap2, useAltMap ? HIGH : LOW); // keyMap2 選択じにON
 
  for(int i=0;i<numButtons;i++){
    bool reading = digitalRead(buttonPins[i]) == LOW;
 
    // ----------------------
    // ディバウンス確認
    // ----------------------
    if(reading != keyStates[i] && (millis() - lastDebounceTimes[i]) > debounceDelay){
      lastDebounceTimes[i] = millis();
      keyStates[i] = reading;
 
      if(keyStates[i]) handlePress(i);
      else handleRelease(i);
 
      // ----------------------
      // Modifier + モード変更感知
      // ----------------------
      if(keyStates[i] && isModifierHeld()){
        if(buttonPins[i] == sequencePins[seqIndex]){
          seqIndex++;
          if(seqIndex == sizeof(sequencePins)/sizeof(sequencePins[0])){
            toggleKeyMap();
            seqIndex = 0;   // 初期化
          }
        } else {
          seqIndex = 0; // 不正解初期化
        }
      }
    }
 
    // ----------------------
    // キーのホールド状態
    // ----------------------
    if(keyStates[i] && (millis() - lastRepeatTimes[i]) > repeatDelay){
      lastRepeatTimes[i] = millis();
      handlePress(i);
    }
  }
}
 
// ----------------------
// keyMap 状態初期化
// ----------------------
void toggleKeyMap() {
  Keyboard.releaseAll();
  Mouse.release(MOUSE_MIDDLE);
  for(int i=0;i<numButtons;i++){
    keyStates[i] = false;
  }
  useAltMap = !useAltMap;
  // LED変更
  digitalWrite(ledPinMap1, useAltMap ? LOW : HIGH);
  digitalWrite(ledPinMap2, useAltMap ? HIGH : LOW);
}
 
// ----------------------
// 音ゲーKeyMap
// ----------------------
void handleBaseMap(int i, bool pressed){
  if(buttonPins[i] == 10){ // ESC
    if(pressed) Keyboard.press(KEY_ESC);
    else Keyboard.release(KEY_ESC);
    return;
  }
  if(buttonPins[i] == 11){ // Enter
    if(pressed) Keyboard.press(KEY_RETURN);
    else Keyboard.release(KEY_RETURN);
    return;
  }
  char key = keyMap1[i];
  if(key == 0) return; // 動作なし
  if(pressed) Keyboard.press(key);
  else Keyboard.release(key);
}
 
// ----------------------
// RPGKeyMap
// ----------------------
void handleAltMap(int i, bool pressed){
  switch(buttonPins[i]){
    case 2: // Q
      if(pressed) Keyboard.press('q'); else Keyboard.release('q');
      break;
    case 3: // E
      if(pressed) Keyboard.press('e'); else Keyboard.release('e');
      break;
    case 4: // R
      if(pressed) Keyboard.press('r'); else Keyboard.release('r');
      break;
    case 5: // マウス左
      if(pressed) Mouse.press(MOUSE_LEFT); else Mouse.release(MOUSE_LEFT);
      break;
    case 6: // t
      if(pressed) Keyboard.press('t'); else Keyboard.release('t');
      break;
    case 8: // Shift
    case 9:
      if(pressed) Keyboard.press(KEY_LEFT_SHIFT); else Keyboard.release(KEY_LEFT_SHIFT);
      break;
    case 10: // ESC
      if(pressed) Keyboard.press(KEY_ESC); else Keyboard.release(KEY_ESC);
      break;
    case 11: // マウスフイール
      if(pressed) Mouse.press(MOUSE_MIDDLE); else Mouse.release(MOUSE_MIDDLE);
      break;
    default:
      // 残りは動作無し
      break;
  }
}
 
// ----------------------
// ボタン押下
// ----------------------
void handlePress(int i){
  if(useAltMap) handleAltMap(i, true);
  else handleBaseMap(i, true);
}
 
// ----------------------
// ボタン引く
// ----------------------
void handleRelease(int i){
  if(useAltMap) handleAltMap(i, false);
  else handleBaseMap(i, false);
}
 
// ----------------------
// Modifier(ESC+Enter) 確認
// ----------------------
bool isModifierHeld(){
  for(int i=0;i<2;i++){
    if(digitalRead(modifierPins[i]) != LOW){
      return false;
    }
  }
  return true;
}