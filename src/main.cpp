#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h> // for Serial
#include <RadioLib.h>

#define RADIOLIB_LR1110_FIRMWARE_0401
// include the firmware image
#include <modules/LR11x0/LR11x0_firmware.h>

LR1110 radio = new Module(LR1110_SPI_NSS_PIN, LR1110_IRQ_PIN, LR1110_NRESET_PIN, LR1110_BUSY_PIN);

void enterDfuMode()
{
    enterUf2Dfu();
}

void printVersions() {
  LR11x0VersionInfo_t version;
  Serial.print(F("[LR1110] Reading firmware versions ... "));
  int16_t state = radio.getVersionInfo(&version);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));

    Serial.print(F("[LR1110] Device: "));
    Serial.println(version.device);

    Serial.print(F("[LR1110] Base firmware: "));
    Serial.print(version.fwMajor);
    Serial.print('.');
    Serial.println(version.fwMinor);

    Serial.print(F("[LR1110] WiFi firmware: "));
    Serial.print(version.fwMajorWiFi);
    Serial.print('.');
    Serial.println(version.fwMinorWiFi);

    Serial.print(F("[LR1110] GNSS firmware: "));
    Serial.print(version.fwGNSS);
    Serial.print('.');
    Serial.println(version.almanacGNSS);

  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  
  }
}

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  Serial.println("LR11x0 Modem firmware updater");
  Serial.println();

  // initialize LR1110 with default settings
  Serial.print(F("[LR1110] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  // print the firmware versions before the update
  printVersions();

  // prompt the user
  Serial.println(F("[LR1110] Send any character to start the update"));
  while(!Serial.available()) { delay(1); }

  // upload update into LR11x0 non-volatile memory
  Serial.print(F("[LR1110] Updating firmware, this may take several seconds ... "));
  state = radio.updateFirmware(lr11xx_firmware_image, RADIOLIB_LR11X0_FIRMWARE_IMAGE_SIZE);
  /*
    use the following if you enabled RADIOLIB_LR1110_FIRMWARE_IN_RAM
    state = radio.updateFirmware(lr11xx_firmware_image, RADIOLIB_LR11X0_FIRMWARE_IMAGE_SIZE, false);
  */
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  // print the firmware versions after the update
  printVersions();

  delay(1); // for message appear on monitor

  enterDfuMode();
}

// the loop function runs over and over again forever
void loop()
{
  // nothing to do
}
