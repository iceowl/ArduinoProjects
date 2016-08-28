#include <AF_Wave.h>
#include <avr/pgmspace.h>
#include "util.h"
#include "wave.h"

const char pi[] PROGMEM = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420198938095257201065485863278865936153381827968230301952035301852968995773622599413891249721775283479131515574857242454150695950829533116861727855889075098381754637464939319255060400927701671139009848824012858361603563707660104710181942955596198946767837449448255379774726847104047534646208046684259069491293313677028989152104752162056966024058038150193511253382430035587640247496473263914199272604269922796782354781636009341721641219924586315030286182974555706749838505494588586926995690927210797509302955321165344987202755960236480665499119881834797753566369807426542527862551818417574672890977772793800081647060016145249192173217214772350141441973568548161361157352552133475741849468438523323907394143334547762416862518983569485562099219222184272550254256887671790494601653466804988627232791786085784383827967976681454100953883786360950680064225125205117392984896084128488626945604241965285022210661186306744278622039194945047123713786960956364371917287467764657573962413890865832645995813390478027590099465764078951269468398352595709825822620522489407726719478268482601476990902640136394437455305068203496252451749399651431429809190659250937221696461515709858387410597885959772975498930161753928468138268683868942774155991855925245953959431049972524680845987273644695848653836736222626099124608051243884390451244136549762780797715691435997700129616089441694868555848406353422072225828488648158456028506016842739452267467678895252138522549954666727823986456596116354886230577456498035593634568174324112515076069479451096596094025228879710893145669136867228748940560101503308617928680920874760917824938589009714909675985261365549781893129784821";

AF_Wave card;
File f;
Wavefile wave;      // only one!

#define redled 9

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Pi speaker");


  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(redled, OUTPUT);
  
  if (!card.init_card()) {
    putstring_nl("Card init. failed!"); return;
  }
  if (!card.open_partition()) {
    putstring_nl("No partition!"); return;
  }
  if (!card.open_filesys()) {
    putstring_nl("Couldn't open filesys"); return;
  }
  if (!card.open_rootdir()) {
    putstring_nl("Couldn't open dir"); return;
  }

  putstring_nl("Files found:");
  ls();

  Serial.println(strlen(pi), DEC);
}

unsigned long digit = 0;

void loop() { 
   char c = pgm_read_byte(&pi[digit++]);
   
   if (c == 0) {
     digit = 0;
     return;
   }
    Serial.print(c);
    
   speaknum(c);
   
   delay(10);
}

char filename[13];
void speaknum(char c) {
  uint8_t i=0;
  
  if (c <= '9' && c >= '0') {
     // single digit
     filename[0] = c;
     i = 1;
  } else if (c == '.') {
    filename[0] = 'P';
    i = 1;
  } else {
    return;
  }
  
  
  if (i != 0) {
    filename[i] = '.';
    filename[i+1] = 'W';
    filename[i+2] = 'A';
    filename[i+3] = 'V';
    filename[i+4] = 0;
  
    playcomplete(filename);
  } 
}


void ls() {
  char name[13];
  card.reset_dir();
  putstring_nl("Files found:");
  while (1) {
    if (!card.get_next_name_in_dir(name)) {
       card.reset_dir();
       return;
    }
    Serial.println(name);
  }
}

void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying);
  card.close_file(f);
}

void playfile(char *name) {
    if (wave.isplaying) {// already playing something, so stop it!
      wave.stop(); // stop it
      card.close_file(f);
   }
   f = card.open_file(name);
   if (!f) {
      putstring("Couldn't open file "); Serial.print(name); return;
   }
   if (!wave.create(f)) {
     putstring_nl("Not a valid WAV"); return;
   }
   // ok time to play!
   wave.play();
}
