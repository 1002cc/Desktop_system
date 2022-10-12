#include "lvgl/lvgl.h"
lv_pinyin_dict_t my_dict[] = {
    {"a", "阿啊呵锕吖腌嗄"},
    {"ai", "爱嫒暧瑷哀锿挨埃诶唉隘嗌艾哎砹癌蔼霭矮碍皑"},
    {"an", "安按桉氨铵庵俺鹌埯胺鞍桉案暗揞黯谙岸"},
    {"ang", "昂盎肮"},
    {"ao", "奥澳懊熬傲嗷敖廒遨鳌骜螯獒岙袄凹坳拗嚣媪"},
    {"ba", "巴岜爸把吧靶耙芭笆疤粑钯八扒叭拔跋魃茇霸灞罢捌坝"},
    {"bai", "白伯柏百佰捭稗败呗拜摆掰"},
    {"ban", "半伴绊拌版板扳坂阪钣舨般搬瘢班斑癍办瓣颁扮"},
    {"bang", "邦绑梆帮榜傍磅膀镑谤蒡棒浜蚌"},
    {"bao", "包饱饱刨胞炮苞鲍龅雹报保褓葆堡煲褒宝薄豹暴爆剥趵鸨"},
    {"bei", "被陂倍焙碚蓓北邶背褙臂杯贝呗狈钡辈悲卑碑萆悖鹎备惫鞴鐾"},
    {"ben", "本苯笨奔贲锛坌夯畚"},
    {"beng", "蚌崩绷蹦嘣泵甭迸甏"},
    {"bi", "匕比毙秕妣吡庇毖毕陛篦蓖必秘铋笔滗臂鼻濞敝弊辟碧璧壁避襞畀裨婢俾髀庳痹弼蔽贲币逼彼闭拂愎鄙"},
    {"bian", "边笾扁编遍鳊褊煸蝙碥匾辩辨辫弁卞汴忭苄便鞭缏变贬砭窆"},
    {"biao", "表裱婊标膘镖骠瘭鳔勺杓彪髟飙镳飑"},
    {"bie", "别鳖憋瘪蹩"},
    {"bin", "宾滨膑殡傧槟镔摈缤髌鬓斌彬濒豳玢"},
    {"bing", "并饼屏摒丙炳柄病兵冰秉禀邴"},
    {"bo", "脖勃渤鹁波玻跛拨菠簸伯铂泊舶柏箔博博膊搏薄播趵驳帛擘檗钹钵踣卜亳般"},
    {"bu", "不钚布怖钸卜卟补捕哺晡埔逋簿埠部步醭瓿"},
    {"ca", "擦嚓礤拆"},
    {"cai", "才材财采菜彩踩睬蔡裁猜"},
    {"can", "参骖掺惨残餐蚕灿惭粲璨"},
    {"cang", "藏仓舱苍沧伧"},
    {"cao", "曹草槽漕嘈螬艚操糙"},
    {"ce", "册策侧测厕恻"},
    {"ceng", "层曾蹭噌"},
    {"ci", "次茨此疵雌词伺祠辞兹慈磁鹚糍刺瓷赐差"},
    {"cong", "从丛枞苁匆葱琮淙囱璁骢聪"},
    {"cou", "凑腠辏楱"},
    {"cu", "粗殂徂促簇醋猝酢蹴蹙"},
    {"cuan", "窜蹿镩撺攒汆篡爨"},
    {"cui", "隹崔催摧璀萃翠粹悴瘁啐淬衰榱脆毳"},
    {"cun", "村寸忖存蹲皴"},
    {"cuo", "错措厝挫矬锉脞搓蹉嵯瘥鹾磋撮"},
    {"cha", "差槎查碴喳馇楂猹茶搽插锸察嚓镲檫叉衩杈汊诧姹茬岔刹"},
    {"chai", "拆差柴钗虿侪豺"},
    {"chan", "产铲单禅蝉婵阐馋搀谗孱潺骣冁冁镡缠颤蟾谄躔廛澶忏蒇羼觇"},
    {"chang", "厂长伥怅苌场肠畅昌唱倡娼猖鲳阊菖尝偿倘徜惝敞氅常嫦裳昶鬯"},
    {"chao", "超吵炒钞抄耖朝潮嘲巢剿绰焯晁怊"},
    {"che", "车砗撤澈扯彻尺坼掣"},
    {"chen", "陈沉尘称臣秤晨谌辰趁衬忱琛橙郴宸碜榇嗔龀伧抻谶"},
    {"cheng", "成城诚铖蛏柽称呈程逞埕裎承盛乘撑澄惩秤橙铛丞枨瞠噌骋酲塍"},
    {"chi", "吃齿持池弛驰尺迟赤哧痴踟耻翅斥叱匙炽豉蚩嗤媸茌墀坻鸱侈眵傺螭魑篪褫啻笞敕饬"},
    {"chong", "冲种忡崇虫充茺宠重憧艟涌舂铳"},
    {"chou", "抽愁瞅丑筹畴帱俦踌绸稠惆仇臭酬雠瘳"},
    {"chu", "出础黜绌刍雏处初除蜍滁楚憷储褚楮躇触锄厨橱蹰矗怵畜搐樗杵"},
    {"chuai", "啜揣踹嘬搋膪"},
    {"chuan", "船传川钏穿串喘遄椽舛氚舡"},
    {"chuang", "创疮怆闯床窗幢"},
    {"chui", "吹炊锤槌垂捶陲棰椎"},
    {"chun", "春椿蝽蠢醇淳纯莼唇鹑"},
    {"chuo", "绰焯踔辍啜戳龊"},
    {"da", "大达哒鞑打答搭嗒沓瘩褡耷疸妲怛靼笪"},
    {"dai", "带代玳戴待贷袋呆岱傣黛甙逮埭殆歹绐骀迨绐怠呔"},
    {"dan", "单弹掸殚惮郸瘅箪旦但担胆疸丹蛋淡啖赕氮诞儋澹耽石萏眈聃"},
    {"dang", "当挡档铛裆谠党荡砀宕菪凼"},
    {"dao", "岛捣到道倒刀叨忉导盗稻蹈悼焘祷帱氘纛"},
    {"de", "的得锝德地"},
    {"deng", "邓灯登澄蹬瞪嶝凳噔簦镫磴戥等"},
    {"di", "帝蒂谛缔碲弟绨娣递睇"},
    {"e", "额饿哦恶呃俄鹅鄂厄讹鳄扼峨遏愕莪噩"},
    {"er", "二而贰耳儿尔饵"},
    {"en","嗯恩摁蒽奀峎煾"},
    {"fa","法发罚伐阀乏筏髪發珐"},
    {"fan","烦返贩饭反翻番犯范凡帆樊泛繁梵幡藩钒矾蕃畈璠犭"},
    {"fang","方邡坊芳枋钫防妨房肪鲂仿访纺昉舫放"},
    {"fei","飞妃非啡婓绯菲扉蜚霏鲱肥淝腓匪诽悱斐榧翡篚吠废沸狒肺费痱镄"},
    {"fen","分吩纷芬氛酚坟汾棼焚鼢粉份奋忿偾愤粪鲼瀵"},
    {"feng","丰风沣沨枫封疯砜峰烽葑锋蜂酆冯逢缝讽唪凤奉俸"},
    {"fo","佛仏坲仸梻"},
    {"fou","否缶"},
    {"fu", "富副福幅辐蝠匐抚赴讣夫扶肤蚨呋芙趺麸蚨负复蝮腹鳆馥覆服付府附俯鲋驸跗符苻孚莩浮俘郛桴咐拊腑蜉孵腐稃阜妇伏袱"
           "茯父斧釜滏甫傅辅脯黼赙缚敷弗佛拂砩怫绋艴氟绂绂祓黻黻赋涪罘芾疠凫菔幞"},
    {"ga", "嘎噶旮伽咖胳轧钆尕尜尬夹"},
    {"gai", "该改盖丐钙概溉赅陔垓芥戤"},
    {"gan", "干杆肝秆矸酐擀竿敢橄澉赶甘柑坩泔旰绀苷疳感赣乾淦尴"},
    {"gang", "冈钢刚纲岗缸扛杠肛罡港筻戆"},
    {"gao", "稿高搞镐缟槁藁篙告郜锆诰膏糕羔杲皋槔睾"},
    {"ge", "个格铬胳各硌骼袼咯阁搁仡虼圪纥屹疙鬲隔镉嗝膈塥哿舸哥歌革葛割盖戈合蛤鸽搿颌"},
    {"gei", "给"},
    {"gen", "艮根跟茛哏亘"},
    {"geng", "更梗埂绠鲠哽耕庚耿赓羹颈"},
    {"gong", "共供拱珙恭工巩红贡汞攻公功宫龚弓躬觥蚣肱"},
    {"gou", "句够狗佝笱苟枸岣勾构购沟钩垢诟觏鞲媾遘篝缑彀"},
    {"gu", "股古故估姑沽钴鸪咕蛄诂牯酤罟菇轱固锢鲴崮痼嘏梏鹄顾谷骨鹘鼓臌雇辜孤觚呱菰贾汩箍蛊瞽毂汨"},
    {"gua", "挂诖卦褂瓜呱胍刮寡括剐鸹"},
    {"guai", "怪拐乖掴"},
    {"guan", "关官馆棺倌涫管观冠莞灌罐贯惯掼鳏纶鹳盥矜"},
    {"guang", "广犷光咣胱桄逛"},
    {"gui", "妫贵归轨圭桂硅鲑闺鬼傀瑰龟柜规炔炅跪诡癸獬庋宄匦刿刽簋桧晷皈鳜gun"},
    {"gun", "衮滚磙棍辊绲鲧"},
    {"guo", "过国帼蝈掴郭果猓蜾呙涡埚锅虢裹椁崞馘聒"},
    {"ha", "哈蛤铪虾"},
    {"hai", "还海害孩亥氦咳嗨骇嘿醢胲骸"},
    {"han", "韩蚶邯酣汉含焓喊憾撼寒汗犴鼾邗顸罕旱焊捍悍撖憨函涵翰瀚阚颔菡晗"},
    {"hang", "行珩绗航杭吭沆巷夯颃"},
    {"hao", "号好浩耗郝豪皓毫昊蒿壕濠蚝嚎貉颢嗥嚆镐薅"},
    {"he", "和河何呵诃蚵嗬荷菏合盒颌核劾阂曷喝褐鹤贺赫禾壑阖涸吓纥貉盍翮"},
    {"hei", "黑嘿嗨"},
    {"hen", "很恨狠痕"},
    {"heng", "恒横亨哼行珩桁衡蘅"},
    {"hong", "哄洪烘红虹讧荭宏闳鸿轰弘泓訇蕻黉薨疫"},
    {"hou", "后逅骺侯候猴喉堠糇篌瘊厚吼後鲎"},
    {"hu", "户护沪戽扈胡湖糊瑚鹕煳醐猢蝴葫虎唬滹琥互乎呼轷烀狐弧瓠壶忽笏惚唿浒核囫鹄祜怙岵和鹱斛槲冱戏鹘觳"},
    {"hua", "划化华骅哗铧桦花画话滑猾豁砉"},
    {"huai", "怀坏淮槐踝徊划"},
    {"huan", "欢环还擐缳圜寰奂换焕涣唤痪患漶桓洹郇缓锾幻宦浣鲩逭鬟豢萑"},
    {"huang", "黄蟥潢璜磺簧癀荒慌谎皇湟煌蝗惶徨隍凰鳇篁遑恍晃幌肓"},
    {"hui", "汇匹会绘烩桧浍荟回洄蛔徊茴辉挥晖珲惠蟪蕙毁彗慧灰恢诙咴悔晦诲溃缋贿麾堕卉秽哕徽讳喙虺恚隳"},
    {"hun", "混馄昏阍婚浑荤珲诨溷魂"},
    {"huo", "或惑后活获货火伙钬和霍嚯藿攉祸豁夥劐镬蠖耠锪"},
    {"i",""},
    {"ji",
     "几机饥矶叽肌玑讥虮及圾极圾汲级芨笈岌己记纪鸡基计集际齐济挤剂跻荠霁哜鲚吉佶诘咭髻继系急积季悸冀籍藉技伎妓芰屐击暨"
     "绩姬箕稽蓟给寄奇畸犄掎迹疾蒺忌跽激祭辑缉楫骥寂脊瘠嵴期稷棘鲫嵇羁麂戟戢蕺革其亟殛洎觊笄齑偈赍即既唧乩丌墼剞畿嫉"},
    {"jia", "家嫁稼镓嘉价加伽珈枷跏架驾笳假茄痂袈迦嘏瘕葭佳甲钾胛岬贾夹郏蛱铗浃颊荚戛袷恝"},
    {"jian", "件建健键腱踺毽楗犍间涧锏裥简见枧舰笕减碱缄兼缣搛蒹鹣检剑捡俭硷睑肩监槛尖坚菅蹇渐鉴剪翦箭湔煎谫拣歼茧荐鞯"
             "柬谏戋浅饯贱践溅笺奸艰謇戬僭鲣囝趼牮"},
    {"jiang", "江豇虹茳将奖浆桨酱蒋强糨犟降洚绛讲姜疆僵缰礓匠耩"},
    {"jiao", "交较胶皎敫蛟绞郊姣跤饺佼狡铰校鲛茭侥浇叫教脚缴徼角焦礁醮噍鹪僬蕉觉搅矫娇轿骄挢峤窖椒嚼剿艽湫纟酵"},
    {"jie", "届孑节疖借街杰接解结洁秸诘鲒桔拮皆喈楷戒截诫捷婕睫揭羯偈竭碣阶介价蚧芥疥骱界姐劫藉家颉嗟桀讦"},
    {"jin", "金仅进尽烬赆荩锦津斤靳近晋缙今衿妗矜劲紧禁襟噤巾浸堇谨瑾槿馑廑筋卺觐"},
    {"jing", "京经晶精竞竟境镜獍泾经劲刭颈径弪茎迳痉井阱肼精靖腈睛婧靓静菁净景憬京惊鲸敬警荆晶兢儆旌粳"},
    {"jiong", "窘迥炯扃炅"},
    {"jiu", "就僦蹴纠赳九旧酒久疚玖救究柩灸揪啾鬏臼桕舅咎鹫鸠厩韭阄"},
    {"ju", "局居据椐踞倨琚裾锯巨拒距炬矩柜钜讵苣榘句驹拘枸锔具俱犋惧飓举榉剧聚菊鞠掬且咀雎龃沮趄狙苴疽遽踽瞿醵莒屦窭橘"
           "鞫桔车"},
    {"juan", "卷倦锩圈眷桊捐娟绢涓狷隽镌俊鹃鄄蠲"},
    {"jue", "决觖诀抉绝孓觉角桷掘倔崛珏爵嚼爝厥劂撅蕨橛镢噘蹶獗矍谲噱攫脚嗟"},
    {"jun", "钧均筠军皲俊峻骏浚竣君郡捃菌龟麇隽"},
    {"ka", "卡咔胩佧喀咯咖"},
    {"kai", "开锎凯恺铠垲剀楷揩锴蒈慨忾"},
    {"kao", "考烤铐栲拷靠犒尻"},
    {"kan", "坎砍嵌莰看刊槛侃堪勘阚瞰龛戡"},
    {"kang", "亢炕抗伉钪闶糠慷康扛"},
    {"ke", "可柯珂坷轲蚵钶苛岢疴克氪科蝌客颗棵课髁锞稞骒窠刻咳颏磕嗑瞌溘壳渴恪缂"},
    {"ken", "肯啃垦恳龈裉"},
    {"keng", "坑吭铿"},
    {"kong", "空控崆倥箜孔恐"},
    {"kou", "口扣筘叩寇蔻抠眍芤"},
    {"ku", "库裤枯酷骷苦哭堀窟绔刳喾"},
    {"kua", "夸挎跨垮胯侉"},
    {"kuai", "筷快块会浍郐侩狯哙脍蒯"},
    {"kuan", "款宽髋"},
    {"kuang", "矿旷邝纩圹匡框眶哐诓筐狂诳况贶夼"},
    {"kui", "亏奎跬喹蝰隗愧傀魁夔睽揆葵愦溃馈聩篑蒉匮窥逵馗喟盔悝暌岿"},
    {"kun", "困捆悃阃昆琨锟鲲醌坤髡"},
    {"kuo", "括蛞栝适阔扩廓"},
    {"la", "拉砬垃啦喇腊蜡落剌辣旯邋瘌"},
    {"lai", "来莱睐铼徕崃涞赉赖濑籁癞"},
    {"lan", "兰栏烂拦蓝滥篮褴岚览揽缆榄阑谰镧斓懒婪漤罱"},
    {"lang", "狼浪郎朗琅锒稂榔螂啷廊阆莨蒗"},
    {"lao", "老姥佬栳铑劳捞涝崂唠耢铹痨牢烙络酪落醪潦"},
    {"le", "了叻仂肋泐勒鳓乐"},
    {"lei", "垒类雷蕾擂镭檑累嫘缧勒嘞泪肋磊羸耒诔酹儡"},
    {"leng", "愣楞塄棱冷"},
    {"li", "丽鹂郦骊俪鲡逦厉疠粝励呖雳砺蛎力历沥坜枥苈疬砾栎轹跞里理哩喱狸鲤锂俚悝厘娌李利俐蜊猁梨犁莉痢立粒莅笠离缡漓"
           "璃篱蓠礼黎藜例栗溧傈篥荔澧醴鳢罹蠡吏隶鬲戾唳嫠詈黧"},
    {"lian", "联连莲链裢涟琏鲢练炼脸殓敛裣潋蔹帘怜廉濂蠊臁镰恋楝奁"},
    {"liang", "两辆俩魉量良粮踉莨梁粱墚亮凉晾谅椋靓"},
    {"liao", "了辽疗钌料寥廖蓼聊撂撩僚燎镣潦缭獠嘹鹩寮"},
    {"lie", "猎列咧洌冽烈裂趔劣鬣躐捩埒"},
    {"lin", "林琳淋啉霖临鳞麟磷膦辚瞵嶙粼遴邻吝蔺赁廪檩凛懔躏拎"},
    {"ling", "令岭领龄铃玲拎伶羚泠呤蛉聆柃瓴翎苓零囹灵棂另陵菱凌绫鲮棱"},
    {"liu", "刘浏六留溜榴骝熘馏镏瘤遛流硫琉旒锍鎏柳陆鹨绺碌"},
    {"lo", "咯"},
    {"long", "龙拢陇胧珑咙泷栊茏笼垄砻聋隆窿癃弄哢"},
    {"lou", "娄楼搂喽镂髅耧偻蝼嵝蒌篓瘘漏陋露"},
    {"lu", "路潞璐露鹭陆卢泸炉鲈栌舻垆胪颅鸬轳驴庐芦卤硵录绿禄碌渌逯鲁橹噜镥撸氇鹿麓辘漉簏戮虏掳辂赂蓼六"},
    {"luan", "栾滦鸾銮峦娈孪挛脔乱卵"},
    {"lun", "仑轮论轮纶伦抡沦囵"},
    {"luo", "罗锣椤猡萝箩逻跞泺洛络硌骆珞烙雒落漯摞螺骡镙瘰裸倮荦脶"},
    {"lv", "律率绿氯吕侣铝闾稆旅膂虑履缕偻褛屡捋"},
    {"lve", "略掠锊"},
    {"ma", "马吗蚂玛码妈犸杩骂麻嘛摩嬷蟆么抹呒唛"},
    {"mai", "买卖荬麦迈劢埋霾脉"},
    {"mao", "毛牦耄髦旄贸锚猫茂冒帽瑁矛茅袤蟊蝥瞀懋卯铆泖茆峁貌昴"},
    {"man", "曼慢漫鳗幔馒熳镘墁谩缦蔓满瞒螨颟蛮埋鞔"},
    {"mang", "忙邙氓芒茫硭莽蟒漭盲"},
    {"me", "麽"},
    {"mei", "每梅酶莓霉没美镁煤媒枚妹昧魅眉湄媚楣猸鹛嵋镅玫寐袂糜谜浼"},
    {"men", "门们闷扪钔焖懑"},
    {"meng", "孟猛锰蜢勐艋梦蒙檬朦盟萌懵氓"},
    {"mi", "米迷咪谜眯脒醚敉麋泌秘谧宓密蜜嘧弥祢猕糜靡縻蘼芈觅汨弭幂"},
    {"miao", "苗瞄描喵鹋庙秒妙杪眇渺缈缪淼邈藐"},
    {"mian", "面缅腼湎棉绵免勉娩冕丐沔眄眠黾渑"},
    {"mie", "灭蔑篾蠛咩乜"},
    {"min", "民泯珉岷抿苠缗敏玟闽闵悯愍黾鳘皿"},
    {"ming", "名铭酩茗明盟鸣命冥溟瞑暝螟"},
    {"miu", "谬缪"},
    {"mo", "末抹沫秣摸莫模膜馍谟漠镆嫫貘瘼寞蓦摹没殁墨耱嬷麽磨摩蘑魔默脉万茉陌貊冒嘿貉无"},
    {"mou", "某谋牟眸哞侔蛑缪鍪蝥"},
    {"mu", "木亩牧母姆拇坶目钼苜幕墓慕募暮模沐穆睦牡姥仫牟毪"},
    {"na", "那哪娜拿镎纳钠呐衲肭捺呢南"},
    {"nai", "耐乃奶艿氖佴奈柰萘鼐哪"},
    {"nao", "脑恼垴瑙硇闹铙挠蛲淖孬猱呶"},
    {"nan", "南楠喃蝻腩难男囡囝赧"},
    {"nang", "囊曩馕囔攮"},
    {"ne", "呢哪讷呐"},
    {"nei", "内馁那哪"},
    {"nen", "嫩恁"},
    {"neng", "能"},
    {"ni", "你祢尼妮泥呢坭铌昵伲怩旎拟倪鲵睨猊霓逆腻溺匿慝"},
    {"nian", "年念捻埝鲶粘拈鲇黏碾廿撵蔫辇"},
    {"niang", "娘酿"},
    {"niao", "鸟袅茑尿脲溺嬲"},
    {"nie", "聂嗫颞蹑镊乜捏涅陧臬镍孽蘖啮"},
    {"nin", "您恁"},
    {"ning", "宁拧柠泞咛聍狞苎凝佞甯"},
    {"niu", "牛妞扭钮纽忸狃拗"},
    {"nou", "耨"},
    {"nong", "农浓侬哝脓弄廾"},
    {"nu", "奴怒努弩胬驽孥"},
    {"nuan", "暖"},
    {"nuo", "挪娜诺锘喏傩糯懦搦"},
    {"nv", "女钕衄恧"},
    {"nve", "虐疟"},
    {"ou", "区欧瓯鸥殴沤呕怄讴偶耦藕"},
    {"pa", "怕帕啪杷钯耙爬筢琶葩扒趴派"},
    {"pai", "牌派哌蒎排俳徘湃拍迫"},
    {"pao", "跑炮泡袍匏咆狍刨疱庖抛脬"},
    {"pan", "盘番潘蟠判泮畔胖袢攀襻拚盼叛扳般磐爿蹒"},
    {"pang", "旁滂磅膀螃耪乓庞胖逄彷"},
    {"pei", "配培陪赔醅锫沛旆霈佩裴胚呸辔帔"},
    {"pen", "喷盆湓"},
    {"peng", "碰彭澎膨嘭蟛朋鹏棚堋捧蓬篷硼怦砰抨烹"},
    {"pi", "批枇毗仳吡纰蚍砒琵庀屁芘皮披陂铍疲丕邳坯坏否痞匹劈啤脾裨蜱睥陴郫埤淠辟僻癖噼擗霹甓譬媲貔堡疋圮罴鼙"},
    {"piao", "票漂螵骠缥嘌嫖瞟飘瓢剽莩殍朴"},
    {"pian", "片扁偏骗犏蹁谝翩篇骈胼便缏"},
    {"pie", "撇瞥氕苤"},
    {"pin", "拼姘品榀聘贫频拚颦嫔苹牝"},
    {"ping", "瓶平评坪鲆枰苹萍凭屏乒俜娉冯"},
    {"po", "破坡颇陂婆繁迫泼魄泊粕珀朴钋皤鄱叵笸钷攴泺钹"},
    {"pou", "剖掊瓿裒"},
    {"pu", "铺浦溥埔脯莆蒲圃匍葡普谱镨氆扑朴仆濮璞蹼噗镤暴瀑曝堡菩"},
    {"qi", "奇骑绮崎琦欹其期旗棋琪祺淇麒骐蜞欺萁葺七气汽齐蛴脐荠企器启岐妻萋凄弃蕲祁乞汔迄讫亓祈圻漆戚槭嘁杞屺岂桤芑起"
           "砌沏歧栖泣契芪憩耆鳍畦稽蹊碛綦俟颀綮缉亟柒"},
    {"qia", "卡恰洽掐髂葜"},
    {"qiao", "乔桥侨峤鞒荞俏悄峭鞘诮巧窍瞧樵憔劁谯雀蕉敲翘橇撬壳锹愀跷硗缲"},
    {"qian", "佥签前钱浅千钎阡纤扦仟迁芊欠肷歉芡潜黔钤铅乾牵谦慊倩嵌遣谴缱钳箝茜骞褰搴掮堑鬈椠愆荨悭犍岍虔"},
    {"qiang", "呛枪抢炝跄羌蜣强襁镪墙嫱樯蔷腔戕戗锖将锵羟蕉"},
    {"qiao", "乔桥侨峤鞒荞敲俏悄峭鞘诮巧瞧樵憔谯劁蕉翘撬橇壳锹愀窍跷硗雀缲"},
    {"qie", "且趄切窃砌郄伽茄锲妾挈怯慊惬箧"},
    {"qin", "衾秦溱螓嗪亲矜琴芩沁吣禽噙擒檎侵寝锓芹钦揿覃勤廑"},
    {"qing", "青清请情晴蜻鲭箐氰圊顷倾倾轻氢罄磬謦庆卿亲綮苘檠黥黧"},
    {"qiong", "穷穹邛筇蛩銎跫琼茕"},
    {"qiu", "求球俅赇逑裘秋鳅楸湫丘邱蚯仇鼽犰酋遒蝤囚泅虬巯糗龟"},
    {"qu", "区驱躯岖去祛取曲蛐趋渠屈瞿蠼衢氍癯璩蘧麴趣娶蛆觑朐劬龋阒鸲磲苣蕖诎戌黢"},
    {"quan", "全诠铨醛辁荃筌痊权泉劝券拳蜷绻圈犬畎颧悛鬈"},
    {"que", "阙却缺炔确雀鹊瘸榷阕悫"},
    {"qun", "裙群麇逡"},
    {"ran", "染然燃冉苒蚺髯"},
    {"rang", "让嚷壤攘穰禳瓤"},
    {"rao", "绕饶桡娆荛扰"},
    {"re", "热若喏惹"},
    {"ren", "人认壬任饪衽妊荏仁刃韧轫纫仞忍稔葚"},
    {"reng", "仍扔"},
    {"ri", "日"},
    {"rong", "容溶熔榕蓉融戎绒狨荣嵘蝾茸冗肜"},
    {"rou", "肉柔揉鞣蹂糅"},
    {"ru", "如洳铷茹入汝儒濡嚅蠕颥襦孺薷乳辱褥溽缛蓐"},
    {"ruan", "软阮朊"},
    {"rui", "瑞芮枘蚋锐睿蕊蕤"},
    {"run", "闰润"},
    {"ruo", "弱若箬偌"},
    {"sa", "洒萨撒卅飒仨挲脎"},
    {"sai", "塞噻赛思腮鳃"},
    {"san", "三散馓霰伞叁糁毵"},
    {"sang", "桑嗓搡磉颡"},
    {"sao", "扫埽嫂骚搔鳋瘙缲臊缫梢"},
    {"se", "色铯瑟塞涩啬穑"},
    {"sen", "森"},
    {"seng", "僧"},
    {"si", "四泗驷斯撕嘶澌死司饲嗣伺笥思锶缌丝咝似寺私姒俟厮肆巳祀汜蛳兕耜鸶厶食厕"},
    {"song", "送宋颂嵩松讼忪凇淞崧菘耸诵悚竦怂"},
    {"sou", "叟搜嗖锼溲馊艘瞍螋飕擞薮嗾嗽"},
    {"su", "苏诉素愫嗉涑觫速溯塑粟僳宿缩俗肃酥簌蔌夙稣谡"},
    {"suan", "算酸蒜狻"},
    {"sui", "岁虽随隋髓睢眭濉穗遂隧燧碎绥荽祟谇尿"},
    {"sun", "孙狲荪损笋榫隼飧"},
    {"suo", "所索嗦唢锁琐缩梭睃羧唆莎桫娑挲嗍蓑"},
    {"sha", "沙纱砂莎痧鲨裟挲杀刹铩啥厦嗄傻唼煞霎杉歃"},
    {"shai", "晒筛色酾"},
    {"shan", "山汕讪舢疝善鄯膳鳝缮蟮单禅掸陕闪扇煽骟衫杉髟钐珊删姗跚栅擅膻嬗潸芟赡剡掺苫埏"},
    {"shang", "上商墒熵绱尚晌垧伤汤觞殇裳赏"},
    {"shao", "少烧召绍韶邵劭苕稍哨捎鞘梢潲蛸艄筲勺杓芍"},
    {"she", "设社蛇舌舍猞射麝涉佘赊畲折厍摄慑滠奢歙赦揲"},
    {"shei", "谁"},
    {"shen", "深身沈神砷甚椹葚绅申审伸胂呻砷婶渖慎莘渗参糁肾矧蜃哂谂诜娠"},
    {"sheng", "生胜牲笙眚省声升盛晟圣绳渑乘剩嵊甥"},
    {"shi", "是市柿铈师饰十士仕什使时鲥埘莳事式拭试轼弑石炻史驶室世视实诗峙侍恃施氏势始食失识狮示湿适释尸拾似逝誓蚀矢屎"
            "螫嗜蓍筮噬虱鲺匙谥贳酾殖嘘豕舐"},
    {"shou", "手收首售守狩寿受授瘦绶兽艏熟"},
    {"shu", "数书树属术沭秫述熟输腧毹叔淑菽暑署曙薯鼠舒抒纾束殳疏蜀孰恕竖殊姝枢梳蔬戍漱澍赎倏庶墅塾摅黍"},
    {"shua", "耍刷唰"},
    {"shuai", "率摔蟀甩帅衰"},
    {"shuan", "拴栓涮闩"},
    {"shuang", "双爽霜孀泷"},
    {"shui", "水税谁睡说"},
    {"shun", "顺舜瞬吮"},
    {"shuo", "说朔搠槊蒴硕数烁铄妁"},
    {"ta", "她他它塔嗒沓踏塌榻蹋鳎溻遢獭漯趿挞闼拓铊"},
    {"tai", "台抬胎跆骀鲐邰苔炱太汰肽酞呔钛态泰薹"},
    {"tan", "摊滩瘫坛覃谭潭镡探弹檀贪坦袒钽谈锬郯碳炭毯痰叹忐坍澹昙"},
    {"tang", "唐糖塘搪瑭溏醣螗堂膛镗樘螳饧汤铴烫棠淌倘躺耥惝趟羰傥帑"},
    {"tao", "讨套涛焘陶掏淘啕萄逃桃洮韬叨滔绦鼗饕"},
    {"te", "特忑忒铽慝"},
    {"teng", "滕藤誊腾疼"},
    {"ti", "体提醍缇题嚏替梯锑踢裼惕剔蹄屉啼涕悌绨剃鹈倜逖荑"},
    {"tian", "天田钿佃畋填忝添舔掭甜恬腆阗殄"},
    {"tiao", "跳挑眺佻祧窕调迢龆苕髫笤条鲦粜蜩"},
    {"tie", "铁贴帖萜餮"},
    {"ting", "厅听廷挺艇铤蜓梃庭莛霆亭停婷葶汀町烃"},
    {"tong", "同铜桐侗酮恫峒垌筒茼通桶捅痛嗵童潼僮瞳统仝砼彤佟恸"},
    {"tou", "头投骰透偷钭"},
    {"tu", "图土吐钍屠涂酴途荼徒兔堍菟突秃凸"},
    {"tuan", "抟团疃彖湍"},
    {"tui", "退腿褪煺推颓蜕忒"},
    {"tun", "屯饨囤吞豚臀褪氽暾"},
    {"tuo", "拖乇托脱拓庹陀沱驼坨砣跎柁酡铊舵佗鸵妥驮魄箨橐柝椭鼍唾"},
    {"u",""},
    {"v",""},
    {"wa", "瓦佤娃洼哇蛙挖袜娲凹腽"},
    {"wai", "外歪崴"},
    {"wan", "万完玩皖脘顽烷莞弯湾宛碗婉琬腕畹豌蜿剜惋菀晚挽娩丸纨芄绾"},
    {"wang", "王汪旺枉往望亡忘芒网罔辋惘魍妄尢"},
    {"wei", "为伪沩维唯惟潍位未味委诿逶萎痿微薇威葳崴卫尾娓艉危魏巍嵬韦伟纬玮炜帏苇违韪围涠闱尉慰蔚畏猥隗喂偎煨隈胃谓渭"
            "猬圩帷桅遗鲔洧"},
    {"wen", "问文纹蚊汶雯温瘟闻稳吻刎璺紊阌"},
    {"weng", "翁瓮嗡蓊蕹"},
    {"wo", "我硪涡蜗窝莴握渥龌喔幄沃卧肟挝斡倭"},
    {"wu", "五伍吾悟捂梧晤唔牾焐浯鼯痦寤无怃妩芜庑吴误蜈乌邬坞钨呜鹜芜勿物芴务雾婺武鹉午仵忤迕屋舞污圬毋侮巫诬恶戊兀杌"
           "阢鋈於"},
    {"xi", "西硒栖牺茜粞舾系奚溪蹊喜禧嬉嘻僖熹戏息熄媳螅洗细锡裼希稀烯唏浠郗席习吸隰袭舄析晰淅蜥皙菥熙昔惜腊悉夕矽汐穸"
           "兮膝玺歙曦羲隙犀铣徙屣阋禊翕醯蟋檄觋欷葸樨蓰饩鼹鼷"},
    {"xia", "下虾吓夏厦峡侠狭硖辖瞎瑕暇遐霞瘕呷狎柙匣罅唬黠"},
    {"xian", "县先洗冼铣跣酰宪筅线现蚬岘限显贤险猃莶仙籼氙鲜藓献咸羡陷馅闲嫌纤跹弦舷衔娴鹇痫掀锨腺暹涎霰祆燹苋见"},
    {"xiang", "向响饷项象橡像蟓乡芗相厢湘缃想箱葙祥详庠香降襄骧镶翔巷享飨鲞"},
    {"xiao", "小校效晓哓骁笑肖销消硝绡蛸削宵霄逍魈孝哮啸萧箫潇筱枭嚣崤淆枵"},
    {"xie", "些写泻谢榭协胁鞋鲑卸渫解蟹携邂懈廨獬斜械邪歇蝎泄绁谐偕血屑挟燮契楔颉缬撷躞薤瀣榍亵勰叶"},
    {"xin", "新薪心芯信欣忻昕辛锌莘鑫馨歆寻衅镡囟"},
    {"xing", "省性姓行荇型星醒腥猩惺形邢刑硎幸悻悻杏荥兴陉擤"},
    {"xiong", "熊雄凶汹兄匈芎胸"},
    {"xiu", "修秀休咻貅鸺庥髹绣锈袖岫宿羞馐臭溴嗅朽"},
    {"xu", "需徐叙溆许浒须顼续序虚嘘墟畜蓄胥婿醑糈旭绪圩吁盱恤戌煦诩栩蓿絮勖酗砉洫"},
    {"xuan", "宣喧煊暄渲楦碹揎萱选旋璇漩轩镟悬玄眩炫铉泫痃癣券绚谖儇"},
    {"xue", "学雪鳕血薛穴靴削谑噱泶踅"},
    {"xun", "讯汛迅训驯逊寻鲟浔荨尊旬询洵徇峋殉恂郇荀巡勋埙循熏醺獯曛薰巽浚荤窨蕈彐"},
    {"ya", "亚娅哑垭桠氩痖压雅鸭押牙蚜伢蚜鸦讶砑迓琊岈芽轧涯睚崖衙丫揠疋埏"},
    {"yan", "严俨酽砚言唁盐焰阎眼烟咽胭燕演验艳滟岩延蜒筵沿铅炎琰剡焱研妍彦谚晏鼹宴堰郾偃颜焉鄢嫣雁奄腌淹掩崦厣罨阉衍闫"
            "厌恹檐赝湮殷魇餍兖芫鼯阽谳阏菸"},
    {"yang", "阳养羊样洋蛘烊徉佯氧痒漾恙央秧泱怏鞅殃鸯杨扬炀疡仰"},
    {"yao", "要腰药姚珧铫耀曜幺么吆窈尧侥瑶摇谣遥鳐崾徭鹞繇窑陶咬邀夭妖杳舀钥约爻肴轺疟"},
    {"ye", "也业叶夜页液掖腋野邺冶爷烨晔哗耶揶椰邪铘噎曳谒靥咽"},
    {"yi",
     "一以苡已异乙亿忆钇屹仡艺呓易埸蜴意臆镱噫薏癔荑痍亦益溢镒嗌缢依铱医宜谊义仪议蚁舣艾衣裔伊咿移移羿弈毅译驿怿峄绎弋"
     "奕翼疑嶷沂遗逸役疫邑挹悒轶佚矣怡眙诒贻饴夷姨胰咦熠抑椅猗倚漪旖欹彝壹殪颐尾迤翌翊黟懿诣揖佾劓刈肄酏翳蛾瘗圯蛇"},
    {"yin", "因姻洇烟铟氤茵引蚓吲印茚银垠龈音喑窨隐尹饮阴荫殷瘾寅夤淫吟鄞霪胤湮堙狺圻"},
    {"ying", "映瑛英应硬迎影鹰盈赢嬴瀛颍颖婴缨樱嘤撄瘿璎鹦营莹荧萦莺萤茔荥蓥滢蝇郢膺楹媵潆罂"},
    {"yo", "哟育唷"},
    {"yong", "用拥佣痈甬涌俑蛹踊勇恿雍臃壅饔永泳咏庸镛慵鳙墉邕喁场"},
    {"you", "有侑铕宥囿由油柚釉铀邮蚰鼬又尢犹优忧尤鱿疣友游蝣右佑幼蚴呦黝幽诱莠悠酉蝤攸猷牖鼢卣莸繇莜"},
    {"yu", "与屿欤于吁纡宇芋竽迂盂余馀狳玉钰鱼渔域雨雩育誉予妤豫预蓣语龉圄谷裕浴峪欲鹆俞喻蝓愉渝榆瑜揄谕觎嵛愈逾窬毓郁"
           "禹俣娱虞羽禺喁隅愚遇寓御狱於淤瘀昱煜舆尉蔚熨聿鹬臾谀腴萸庾瘐燠驭窳菀蜮妪伛圉鬻阈舁饫畲粥"},
    {"yuan", "元沅垸芫远园鼋原源塬螈愿员圆院袁猿辕缘掾橼垣渊苑箢爰援媛瑗怨眢冤鸳鸢圜"},
    {"yue", "月刖钥越樾约跃岳粤乐栎悦说阅曰钺瀹龠哕"},
    {"yun", "云耘酝纭运芸员陨殒郧恽郓晕愠韫蕴氲孕允狁匀韵均昀筠熨"},
    {"za", "杂扎咋匝砸咂拶"},
    {"zai", "在再哉栽载灾宰仔崽甾"},
    {"zan", "咱昝赞攒瓒簪趱拶暂糌錾"},
    {"zang", "脏臧藏葬赃奘驵"},
    {"zao", "枣早造糟遭灶凿噪澡躁燥缲藻皂唣蚤"},
    {"ze", "则侧泽择责赜啧帻箦咋舴迮笮仄昃"},
    {"zei", "贼"},
    {"zen", "怎谮"},
    {"zeng", "曾增赠憎缯罾综甑"},
    {"zi", "自子籽仔耔字资粢姿咨恣谘趑紫訾滋嵫孳淄鲻锱缁辎孜梓秭兹渍吱姊眦呲龇茈赀觜髭笫滓"},
    {"zong", "总宗综纵棕踪粽腙鬃偬枞"},
    {"zou", "走邹驺奏揍陬诹鲰鄹"},
    {"zu", "足组租祖阻俎诅菹族镞卒"},
    {"zuan", "钻赚攥纂缵躜"},
    {"zui", "最蕞罪觜嘴醉咀"},
    {"zun", "尊撙鳟樽遵"},
    {"zuo", "作昨柞祚怍胙阼笮做坐唑座左佐撮嘬琢凿"},
    {"zha", "扎轧札蜡乍炸诈咋蚱柞砟痄榨查楂渣揸喳齄闸咤栅眨铡吒喋哳"},
    {"zhai", "寨债摘翟祭瘵砦窄宅斋侧择"},
    {"zhan", "占站粘沾战毡展搌辗湛詹瞻谵斩崭颤栈盏蘸绽旃"},
    {"zhang", "长张账胀帐涨章蟑障漳樟璋幛嶂獐嫜彰鄣瘴仉掌丈仗杖"},
    {"zhao", "找赵着照兆朝嘲召昭招沼诏罩棹肇钊爪笊啁"},
    {"zhe", "这着者锗赭折浙哲蜇蛰蔗鹧遮辙柘辄谪磔褶"},
    {"zhei", "这"},
    {"zhen", "真镇缜稹针甄振赈震珍诊疹轸胗畛阵贞桢侦祯浈帧圳枕臻榛溱蓁斟椹箴鸩朕砧"},
    {"zheng", "正证政征怔钲整症郑争挣睁铮峥诤狰筝丁蒸拯鲭"},
    {"zhi", "至侄桎轾蛭致窒膣之芝只识职织帜轵枳制志支枝忮吱肢治直值植殖埴置质踬纸旨酯指脂知蜘智执止址趾祉芷滞郅汁掷踯炙"
            "稚峙痔雉挚秩栉痣彘骘贽鸷帙黹豸跖絷徵摭咫祗胝觯卮栀瘛氏"},
    {"zhong", "中忠种钟肿仲舯盅众重锺踵终衷冢忪螽"},
    {"zhou", "州洲周啁舟粥骤皱轴妯宙肘昼绉诌胄咒纣酎荮帚籀碡繇"},
    {"zhu", "主住驻柱注拄蛀炷疰诸猪渚褚潴槠煮箸著翥铸伫贮术橥朱珠株蛛侏诛洙铢邾茱助竹筑逐祝属嘱瞩躅烛竺杼舳苎麈瘃"},
    {"zhua", "爪抓挝"},
    {"zhuai", "拽转"},
    {"zhuan", "专转砖传啭撰馔赚篆沌颛"},
    {"zhuang", "装庄壮桩幢撞僮状妆奘戆"},
    {"zhui", "追缒坠椎锥骓隹缀惴"},
    {"zhun", "准谆屯肫窀"},
    {"zhuo", "着卓桌倬焯镯灼酌捉浊拙茁涿琢啄诼濯擢浞禚斫缴"},
    {"zi", "吱兹咨资姿滋淄孜紫仔籽滓刟肎渍夤谘茈嵫姊孳缁梓辎赀恣眦锱秭耔笫粢趑觜訾龇鲻髭"},
    {"zong", "鬃棕踪宗综总纵偬枞腙粽"},
    {"zou", "邹走奏揍诹陸鄹驺鲰"},
    {"zu", "租足卒族祖诅阻组俎菹镞"},
    {"zuan", "钻纂攥缵躜"},
    {"zui", "嘴醉最罪蕞觜"},
    {"zun", "尊遵撙樽鳟"},
    {"zuo", "撮琢昨左佐柞做作坐座阼唑嘬怍胙祚砟酢"},
    {NULL, NULL}

};