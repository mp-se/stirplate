EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RF_Module:ESP-12E U1
U 1 1 5FF4A958
P 3150 2000
F 0 "U1" H 3150 2981 50  0000 C CNN
F 1 "ESP-12E" H 3150 2890 50  0000 C CNN
F 2 "RF_Module:ESP-12E" H 3150 2000 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 2800 2100 50  0001 C CNN
	1    3150 2000
	1    0    0    -1  
$EndComp
$Comp
L Interface_Expansion:PCF8574 U2-LCD
U 1 1 5FF4BECB
P 4900 1900
F 0 "U2-LCD" H 4900 2781 50  0000 C CNN
F 1 "PCF8574" H 4900 2690 50  0000 C CNN
F 2 "" H 4900 1900 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/PCF8574_PCF8574A.pdf" H 4900 1900 50  0001 C CNN
	1    4900 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5FF4D0CC
P 1100 3250
F 0 "#PWR?" H 1100 3100 50  0001 C CNN
F 1 "+12V" H 1115 3423 50  0000 C CNN
F 2 "" H 1100 3250 50  0001 C CNN
F 3 "" H 1100 3250 50  0001 C CNN
	1    1100 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT R2
U 1 1 5FF50DCE
P 1850 1800
F 0 "R2" H 1780 1846 50  0000 R CNN
F 1 "5k" H 1780 1755 50  0000 R CNN
F 2 "" H 1850 1800 50  0001 C CNN
F 3 "~" H 1850 1800 50  0001 C CNN
	1    1850 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R1
U 1 1 5FF51542
P 1850 1400
F 0 "R1" H 1909 1446 50  0000 L CNN
F 1 "2.2k" H 1909 1355 50  0000 L CNN
F 2 "" H 1850 1400 50  0001 C CNN
F 3 "~" H 1850 1400 50  0001 C CNN
	1    1850 1400
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:LF50_TO220 U3
U 1 1 5FF51C59
P 1950 3250
F 0 "U3" H 1950 3492 50  0000 C CNN
F 1 "L4940V5 TO-220 " H 1950 3401 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 1950 3475 50  0001 C CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/c4/0e/7e/2a/be/bc/4c/bd/CD00000546.pdf/files/CD00000546.pdf/jcr:content/translations/en.CD00000546.pdf" H 1950 3200 50  0001 C CNN
	1    1950 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5FF4D91A
P 3150 3150
F 0 "#PWR?" H 3150 2900 50  0001 C CNN
F 1 "GND" H 3155 2977 50  0000 C CNN
F 2 "" H 3150 3150 50  0001 C CNN
F 3 "" H 3150 3150 50  0001 C CNN
	1    3150 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 3150 3150 3050
Wire Wire Line
	4900 2600 4900 3050
Connection ~ 3150 3050
Wire Wire Line
	1950 3550 2350 3550
Wire Wire Line
	3750 2200 3850 2200
Wire Wire Line
	3850 2200 3850 2900
Wire Wire Line
	3150 1200 4900 1200
Connection ~ 3150 1200
Wire Wire Line
	3750 1900 4050 1900
Wire Wire Line
	4050 1900 4050 1500
Wire Wire Line
	4050 1500 4400 1500
Wire Wire Line
	3750 1800 4150 1800
Wire Wire Line
	4150 1800 4150 1600
Wire Wire Line
	4150 1600 4400 1600
Wire Wire Line
	3150 2700 3150 3050
Wire Wire Line
	2250 3250 2450 3250
Wire Wire Line
	2450 3250 2450 1200
Wire Wire Line
	2450 1200 3150 1200
Wire Wire Line
	1100 3250 1300 3250
Wire Wire Line
	1700 2550 1700 2600
Wire Wire Line
	1700 2600 2350 2600
Wire Wire Line
	2350 2600 2350 3050
Wire Wire Line
	1700 2050 1300 2050
Wire Wire Line
	1300 2050 1300 3250
Connection ~ 1300 3250
Wire Wire Line
	1300 3250 1650 3250
Wire Wire Line
	3150 3050 4900 3050
Wire Wire Line
	3150 3050 2350 3050
Connection ~ 2350 3050
Wire Wire Line
	2350 3050 2350 3550
Wire Wire Line
	1850 1300 1850 1200
Wire Wire Line
	1850 1200 2450 1200
Connection ~ 2450 1200
Wire Wire Line
	1850 1650 1850 1500
Wire Wire Line
	2550 1800 2000 1800
Wire Wire Line
	1850 1950 1850 2050
Wire Wire Line
	1850 2050 2350 2050
Wire Wire Line
	2350 2050 2350 2600
Connection ~ 2350 2600
Text Notes 2000 1750 0    50   ~ 0
Limit to 3.3V
Text Notes 4000 2950 0    50   ~ 0
GPIO5  - D1 - SCL\nGPIO4  - D2 - SDA\nGPIO14 - D5 \nGPIO12 - D6\nADC    - A0
Text Notes 2500 3250 0    50   ~ 0
+5V
$Comp
L Motor:Fan_4pin FAN
U 1 1 5FF4FFCA
P 1700 2350
F 0 "FAN" H 1858 2446 50  0000 L CNN
F 1 "Fan_4pin" H 1858 2355 50  0000 L CNN
F 2 "" H 1700 2360 50  0001 C CNN
F 3 "http://www.formfactors.org/developer%5Cspecs%5Crev1_2_public.pdf" H 1700 2360 50  0001 C CNN
	1    1700 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2000 3900 2000
Wire Wire Line
	3900 2000 3900 2950
Wire Wire Line
	3900 2950 1350 2950
Wire Wire Line
	1350 2950 1350 2250
Wire Wire Line
	1350 2250 1400 2250
Wire Wire Line
	1400 2900 1400 2450
Wire Wire Line
	1400 2900 3850 2900
$Comp
L Device:R_Small R3
U 1 1 600B4575
P 1350 1400
F 0 "R3" H 1409 1446 50  0000 L CNN
F 1 "3.3k" H 1409 1355 50  0000 L CNN
F 2 "" H 1350 1400 50  0001 C CNN
F 3 "~" H 1350 1400 50  0001 C CNN
	1    1350 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 2250 1350 1500
Connection ~ 1350 2250
Wire Wire Line
	1350 1300 1350 1200
Wire Wire Line
	1350 1200 1850 1200
Connection ~ 1850 1200
$EndSCHEMATC
