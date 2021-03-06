
#define CONFIG_MAGIC 0x43

typedef struct {
  uint8_t magic;
  char blynkToken[33];
} Config;

Config config;

void configSetup() {
  EEPROM.begin(512);
  configRead();
}

void configWrite() {
  logInfo("Writing config");

  config.magic = CONFIG_MAGIC;
  EEPROM.put(0, config);
  EEPROM.commit();
}

void configRead() {
  logInfo("Reading config");

  EEPROM.get(0, config);
  if (config.magic != CONFIG_MAGIC) {
    logInfo("Unable to read config");
    
    memset(&config, 0, sizeof(config));
    
    configWrite();
  }

  logValue("  token ",config.blynkToken);
}
