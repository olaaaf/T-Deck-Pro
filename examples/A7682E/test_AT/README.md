## Some AT command test

| command      | explain                              | Response     |
| ------------ | ------------------------------------ | ------------ |
| AT           | AT test instruction                  | OK           |
| ATI          | version information                  |              |
| AT+CPIN?     | Example Query the SIM card status    | +CPIN: READY |
| AT+CGREG?    | Network Registration Status          | +CGREG: 0,1  |
| AT+CSQ       | Network signal quality query         |              |
| AT+COPS?     | Querying current Carrier Information |              |

Refer to the T-A7670X instruction manual for more [AT instructions](../../../hardware/A76XX_series/A76XX_Series_AT_Command_Manual_V1.09.pdf) 


## File System


## Audio AT test

1. `AT+CTTSPARAM=1,3,0,1,1` : TTS parameters setting
2. `AT+CTTS=2,"1234567890"`：synth and play ASCII text

AT+CTTSPARAM=1,3,0,1,1,1
AT+CTTS=2,"1234567890"


AT+CCMXPLAY="c:/iPhone_Ring.mp3",0,255
AT+CCMXSTOP

More audio application references [A76XX Series_Audio_Application Note](../../../hardware/A76XX_series/A76XX%20Series_Audio_Application%20Note_V1.03.pdf)








