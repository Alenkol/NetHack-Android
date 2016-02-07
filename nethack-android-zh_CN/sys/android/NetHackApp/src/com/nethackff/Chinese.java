package com.nethackff;

public class Chinese {
	/*
	 * ���ȣ��ⲻ��һ���÷�����
	 * ����NetHackԴ�ļ��������Ϣ���������
	 * ���˲���ת�����룬�������ô˷�����
	 * Yuer
	 */
	private int pro=0;  //��������

	public String replace(String s)
	{
		if(s.length()==0) return s;
		if(s.matches(".{1,2}[0-9]{1,2};[0-9]{1,2}.{1,2}")) return s;
		System.out.println(s.length()+s);
		s=gameStart(s);
		if(pro>=10&&pro<20) s=gameIn(s);
		if(pro>=20) s=gameEnd(s);
		return s;
	}
	
	private String gameStart(String s)
	{
		int t=-1;
		t=s.indexOf("Who are you");
		if(t>=0) pro=1;
		t=s.indexOf("Shall I pick");
		if(t>=0) pro=1;
		t=s.indexOf("There are files");
		if(t>=0) pro=1;
		t=s.indexOf("Checkpointing was");
		if(t>=0) pro=1;
		t=s.indexOf("Pick a role");
		if(t>=0) pro=2;
		t=s.indexOf("Pick the race");
		if(t>=0) pro=3;
		t=s.indexOf("Pick the gender");
		if(t>=0) pro=4;
		t=s.indexOf("Pick the alignment");
		if(t>=0) pro=5;
		t=s.indexOf("It is wr");
		if(t>=0) pro=6;
		t=s.indexOf("a newly");
		if(t>=0) pro=6;
		t=s.indexOf("Go bravely with");
		if(t>=0) pro=6;
		t=s.indexOf("Aloha");
		if(t>=0) pro=24;
		t=s.indexOf("welcome");
		if(t>=0) pro=7;
		t=s.indexOf("tonight");
		if(t>=0) pro=7;
		t=s.indexOf("St:");
		if(t>=0) pro=7;
		t=s.indexOf("Dlvl:");
		if(t>=0) pro=7;
		t=s.indexOf("Do you");
		if(t>=0) pro=20;
		t=s.indexOf("Final Att");
		if(t>=0) pro=21;
		t=s.indexOf("Vanquished");
		if(t>=0) pro=22;
		t=s.indexOf("Voluntary");
		if(t>=0) pro=23;
		t=s.indexOf("Goodbye");
		if(t>=0) pro=24;
		t=s.indexOf("Fare thee well");
		if(t>=0) pro=24;
		t=s.indexOf("Sayonara");
		if(t>=0) pro=24;
		t=s.indexOf("when you");
		if(t>=0) pro=24;
		t=s.indexOf("No  P");
		if(t>=0) pro=25;
		t=s.indexOf("Killed by");
		if(t>=0) pro=25;
		
		if(pro==1)
		{
			s=s.replaceAll("Who are you", "�� �� ʲ ô ");
			s=s.replaceAll("Shall I pick a character's race, role, gender and alignment for you", 
				"Ҫ �� Ϊ �� �� �� ѡ �� �� ɫ �� ְ ҵ �� �� �� �� �� �� �� �� Ӫ �� ");
			s=s.replaceAll("There are files from a game in progress under your name. Recover", 
					"�� �� �� �� �� �� �� �� Ϸ �� �� �� ¼ �� Ҫ �� �� �� ");
			s=s.replaceAll("Checkpointing was not in effect for ", 
					"�� �� �� �� �� �� �� �� Ч ");
			s=s.replaceAll("recovery impossible", 
					"�� �� ʧ �� ");
			s=s.replaceAll("Couldn't recover old game. Destroy it", 
					"�� �� �� �� �� Ҫ ɾ �� �� ");
		}
		if(pro==2)
		{
			s=s.replaceAll("Pick a role for your character", "ѡ �� ְ ҵ ");
			s=s.replaceAll("- a ", "- ");
			s=s.replaceAll("- an ", "- ");
			s=character(s);
			s=s.replaceAll("Random", "�� �� ");
			s=s.replaceAll("Quit", "�� �� ");
		}
		if(pro==3)
		{
			s=s.replaceAll("Pick the race of your ", "ѡ �� �� �� -- Ϊ �� �� ");
			s=s.replaceAll("- a ", "- ");
			s=s.replaceAll("- an ", "- ");
			s=character(s);
			s=s.replaceAll("Random", "�� �� ");
			s=s.replaceAll("Quit", "�� �� ");
		}
		if(pro==4)
		{
			s=s.replaceAll("Pick the gender of your ", "ѡ �� �� �� -- Ϊ �� �� ");
			s=s.replaceAll("- a ", "- ");
			s=s.replaceAll("- an ", "- ");
			s=character(s);
			s=s.replaceAll("Random", "�� �� ");
			s=s.replaceAll("Quit", "�� �� ");
		}
		if(pro==5)
		{
			s=s.replaceAll("Pick the alignment of your ", "ѡ �� �� Ӫ -- Ϊ �� �� ");
			s=s.replaceAll("- a ", "- ");
			s=s.replaceAll("- an ", "- ");
			s=character(s);
			s=s.replaceAll("Random", "�� �� ");
			s=s.replaceAll("Quit", "�� �� ");
		}
		if(pro==6)
		{
			if(NetHackGame.pres!=null)
			{
				s=NetHackGame.pres+s;
				NetHackGame.pres=null;
				//System.out.println(s);
			}
			if(s.indexOf("It is")==-1||s.indexOf("--More--")==-1)
			{
				NetHackGame.pres=s;
				s="";
				return s;
			}
			s=s.replaceAll("It is written in the Book of ", "�� �� д �� �� �� Ϊ ");
			s=s.replaceAll("After the Creation, the cruel god Moloch rebelled", 
					"�� �� �� �� ֮ �� �� �� �� �� Ħ �� �� ı �� �� ");
			s=s.replaceAll("against the authority of Marduk the Creator.", 
					"�� �� �� �� �� �� �� �� �� �� ");
			s=s.replaceAll("Moloch stole from Marduk the most powerful of all", 
					"Ħ �� �� �� �� �� �� �� �� �� �� �� �� ͵ �� �� ");
			s=s.replaceAll("the artifacts of the gods, the Amulet of Yendor,", 
					"�� Ϊ ǿ �� �� �� �� �� �� �� �� ");
			s=s.replaceAll("and he hid it in the dark cavities of Gehennom, the", 
					"�� �� �� �� �� �� �� �� �� �� �� ŵ ķ �� ");
			s=s.replaceAll("Under World, where he now lurks, and bides his time.", 
					"�� �� �� �� �� �� �� �� ʱ �� �� �� �� �� ");
			s=s.replaceAll("Your god ", "�� �� �� ");
			s=s.replaceAll("Your goddess ", "�� �� Ů �� ");
			s=s.replaceAll(" seeks to possess the Amulet, and with it", 
					"�� Ҫ �� �� �� �� ˵ �� �� �� �� �� �� �� �� �� �� Ϊ �� ");
			s=s.replaceAll("to gain deserved ascendance over the other gods.", 
					"�� �� �� �� �� �� �� ռ �� �� �� λ �� ");
			s=s.replaceAll("You, a newly trained ", "�� �� �� �� ֻ �� �� �� �� ");
			s=s.replaceAll(", have been heralded", "�� �� �� �� �� С ");
			s=s.replaceAll("from birth as the instrument of ", "�� �� �� �� �� Ϊ ");
			s=s.replaceAll(".  You are destined", "�� �� ʵ ū �� �� ");
			s=s.replaceAll("to recover the Amulet for your deity, or die in the", 
					"�� �� �� �� �� �� ע �� Ҫ Ϊ �� �� �� ȡ �� �� �� �� �� �� �� ");
			s=s.replaceAll("attempt.  Your hour of destiny has come.  For the sake", 
					"�� �� �� һ �� �� �� �� �� �� ");
			s=s.replaceAll("of us all:  Go bravely with ", 
					"Ϊ �� �� �� �� �� �� �� �� �� �� ǰ �� �� �� ");
			s=god(s);
			s=newly(s);
			s=s.replaceAll("More", "�� �� ");
		}
		if(pro==7)
		{
			s=s.replaceAll("Velkommen ", "�� �� ");
			s=s.replaceAll("Hello ", "�� �� ");
			s=s.replaceAll("Aloha ", "�� �� ");
			s=s.replaceAll("Konnichi wa ", "�� �� ");
			s=s.replaceAll("Salutations ", "�� �� ");
			s=s.replaceAll("welcome to NetHack! You are a ", 
					"�� ӭ �� �� �� �� �� �� �� �� �� һ λ ");
			s=s.replaceAll("welcome back to", "�� ӭ �� �� ");
			s=s.replaceAll("NetHack", "�� �� �� �� ");
			s=s.replaceAll("Be careful!", "Ҫ С �� ��");
			s=s.replaceAll("Becareful!", "Ҫ С �� ��");
			s=s.replaceAll("New moon tonight.", "�� �� �� �� �� ");
			s=s.replaceAll("the ", "");
			s=character(s);
			s=att(s);
		}
		if(pro==0) pro=10;
		return s;
	}
	
	private String gameIn(String s)
	{
		int t=-1;
		t=s.indexOf("Weapons");
		if(t>=0) s=weapon(s);
		t=s.indexOf("Armor");
		if(t>=0) s=armor(s);
		t=s.indexOf("Comestibles");
		if(t>=0) s=comesti(s);
		t=s.indexOf("Scrolls");
		if(t>=0) s=scroll(s);
		t=s.indexOf("Spellbooks");
		if(t>=0) s=spellbook(s);
		t=s.indexOf("Potions");
		if(t>=0) s=potion(s);
		t=s.indexOf("Rings");
		if(t>=0) s=ring(s);
		t=s.indexOf("Wands");
		if(t>=0) s=wand(s);
		t=s.indexOf("Tools");
		if(t>=0) s=tool(s);
		t=s.indexOf("Gems");
		if(t>=0) s=gem(s);
		t=s.indexOf("Statues");
		if(t>=0) s=statue(s);
		t=s.indexOf("Currently known spells");
		if(t>=0) pro=11;
		t=s.indexOf("Choose which spell");
		if(t>=0) pro=11;
		t=s.indexOf("Base Att");
		if(t>=0) pro=12;
		
		if(pro==11)
		{
			s=spell(s);
		}
		if(pro==12)
		{
			s=s.replaceAll("Base Attributes", "�� �� �� �� ");
			s=s.replaceAll("Starting", "�� ʼ.");
			s=s.replaceAll("Current", "�� ǰ.");
			s=s.replaceAll("Deities", "�� ��.");
			s=s.replaceAll("name", "�� �� ");
			s=s.replaceAll("race", "�� �� ");
			s=s.replaceAll("role", "ְ ҵ ");
			s=s.replaceAll("gender", "�� ��   ");
			s=s.replaceAll("alignment", "�� Ӫ      ");
			s=character(s);
			s=s.replaceAll("Chaotic", "�� ��    ");
			s=s.replaceAll("Neutral", "�� ��    ");
			s=s.replaceAll("Lawful", "�� ��   ");
			s=god(s);
		}
		s=info(s);
		return s;
	}
	
	private String gameEnd(String s)
	{
		s=s.replaceAll("Do you want your", "�� �� ȷ �� �� ");
		s=s.replaceAll("possessions", "�� �� ");
		s=s.replaceAll("identified", "�� Ʒ �� ");
		s=s.replaceAll("Do you want to see your attributes", "�� �� �� �� �� �� �� �� �� ");
		s=s.replaceAll("Do you want an account of creatures vanquished", 
				"�� �� �� �� �� �� �� �� �� �� ");
		s=s.replaceAll("Do youwant an account of creatures vanquished", 
				"�� �� �� �� �� �� �� �� �� �� ");
		s=s.replaceAll("Do you want to see your conduct", "�� �� �� �� �� �� �� �� �� ");
		s=s.replaceAll("Do youwant to see your conduct", "�� �� �� �� �� �� �� �� �� ");
		
		if(pro==21)
		{
			s=s.replaceAll("Final Attributes", "�� �� �� �� ");
		}
		if(pro==22)
		{
			s=s.replaceAll("Vanquished creatures", "�� �� �� �� �� �� ");
			s=monster(s);
			s=s.replaceAll("creatures vanquished", "�� �� �� �� �� �� ");
		}
		if(pro==23)
		{
			s=s.replaceAll("Voluntary challenges", "�� �� �� ս ");
		}
		if(pro==24)
		{
			if(NetHackGame.pres!=null)
			{
				s=NetHackGame.pres+s;
				NetHackGame.pres=null;
			}
			if(s.indexOf("...")==-1||s.indexOf("--More--")==-1)
			{
				int t=s.indexOf("level");
				if(t==-1) return s;
				NetHackGame.pres=s;
				s="";
				return s;
			}
			s=s.replaceAll(" killed by a", "�� �� �� �� �� ɱ ");
			s=s.replaceAll("Goodbye ", "�� �� ");
			s=s.replaceAll("Fare thee well ", "�� �� ");
			s=s.replaceAll("Aloha ", "�� �� ");
			s=s.replaceAll("Sayonara ", "�� �� ");
			s=s.replaceAll("the ", "");
			s=character(s);
			s=s.replaceAll("You quit in The Dungeons of Doom ", 
					"�� �� ĩ �� �� �� �� �� �� �� �� ");
			s=s.replaceAll("You died in The Dungeons of Doom ", 
					"�� �� �� ĩ �� �� �� �� �� ");
			s=s.replaceAll("on dungeon level ", "�� �� �� �� ");
			s=s.replaceAll("with ", "�� ");
			s=s.replaceAll("and ", "�� ");
			s=s.replaceAll(" pieces of gold", "�� �� ");
			s=s.replaceAll("after ", "�� �� ");
			s=s.replaceAll(" moves", "�� ");
			s=s.replaceAll(" move", "�� ");
			s=s.replaceAll("You were level ", "�� �� �� �� ");
			s=s.replaceAll("a maximum of ", "�� �� ");
			s=s.replaceAll(" hit points ", "HP  ");
			s=s.replaceAll(" points", "�� �� ");
			s=s.replaceAll("when you died", "�� �� �� �� ʱ �� ");
			s=s.replaceAll("when you quit", "�� �� �� �� �� ʱ �� ");
			s=s.replaceAll("REST", " ��  ");
			s=s.replaceAll("IN", "Ϣ ");
			s=s.replaceAll("PEACE", "  �� ");
			s=s.replaceAll("Au", "�� ");
			s=monster(s);
		}
		if(pro==25)
		{
			s=s.replaceAll("You made the top ten list", "�� �� �� �� ǰ ʮ ");
			s=s.replaceAll("No  ", "�� �� ");
			s=s.replaceAll("Points    ", "      �� �� ");
			s=s.replaceAll("Name", "�� �� ");
			s=chalist(s);
			s=s.replaceAll("quit in The Dungeons of Doom on", 
					"�� ĩ �� �� �� �� �� �� �� �� ");
			s=s.replaceAll("died in The Dungeons of Doom on", 
					"�� �� ĩ �� �� �� �� �� ");
			s=s.replaceAll("level ", "�� �� ");
			s=s.replaceAll("Killed by a ", "�� �� �� �� �� ɱ ");
		}
		s=s.replaceAll("More", "�� �� ");
		return s;
	}
	
	private String att(String s)
	{
		s=s.replaceAll("St:", "�� :");
		s=s.replaceAll("Dx:", "�� :");
		s=s.replaceAll("Co:", "�� :");
		s=s.replaceAll("In:", "�� :");
		s=s.replaceAll("Wi:", "�� :");
		//s=s.replaceAll("Ch:", "�� :"); //�޷��滻����Ϊʲô��
		//s=s.replaceAll("Lawful", "�� ��");
		//s=s.replaceAll("Neutral", "�� ��");
		//s=s.replaceAll("Chaotic", "�� ��");
		s=s.replaceAll("Dlvl:", "�� �� :");
		s=s.replaceAll("Pw:", "Mp:");
		//s=s.replaceAll("AC:", "�� :");
		s=s.replaceAll("Exp:", " Lv:");
		//s=s.replaceAll("Satiated", "�� �� �� ");
		return s;
	}
	
	private String weapon(String s)
	{
		s=s.replaceAll("Weapons", "�� ��.");
		s=s.replaceAll("weapon in hands", "˫ �� �� �� ");
		s=s.replaceAll("weapon in hand", "�� �� ");
		s=s.replaceAll("alternate weapon; not wielded", "�� �� ");
		s=s.replaceAll("in quiver", "׼ �� ");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String armor(String s)
	{
		s=s.replaceAll("Armor", "�� ��.");
		s=s.replaceAll("being worn", "�� �� ");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String comesti(String s)
	{
		s=s.replaceAll("Comestibles", "ʳ ��.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String scroll(String s)
	{
		s=s.replaceAll("Scrolls", "�� ��.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String spellbook(String s)
	{
		s=s.replaceAll("Spellbooks", "ħ �� ��.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String potion(String s)
	{
		s=s.replaceAll("Potions", "ҩ ˮ.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String ring(String s)
	{
		s=s.replaceAll("Rings", "�� ָ.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=s.replaceAll("on right hand", "�� �� �� ");
		s=s.replaceAll("on left hand", "�� �� �� ");
		s=state(s);
		return s;
	}
	
	private String wand(String s)
	{
		s=s.replaceAll("Wands", "ħ ��.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String tool(String s)
	{
		s=s.replaceAll("Tools", "�� ��.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String gem(String s)
	{
		s=s.replaceAll("Gems", "�� ʯ.");
		s=s.replaceAll("in quiver", "׼ �� ");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String statue(String s)
	{
		s=s.replaceAll("Boulders/Statues", "�� ʯ / �� ��.");
		s=s.replaceAll("- an ", "- ");
		s=s.replaceAll("- a ", "- ");
		s=state(s);
		return s;
	}
	
	private String spell(String s)
	{
		s=s.replaceAll("Currently known spells", "�� ѧ ħ �� ");
		s=s.replaceAll("Choose which spell to cast", "ѡ �� Ҫ ʩ չ �� ħ �� ");
		s=s.replaceAll("Name", "�� �� ");
		s=s.replaceAll("Level", "�� ��  ");
		s=s.replaceAll("Category", "�� �� ");
		s=s.replaceAll("Fail", "ʧ �� �� �� ");
		s=s.replaceAll("attack", "�� ��   ");
		s=s.replaceAll("healing", "�� ��    ");
		s=s.replaceAll("clerical", "�� ʥ     ");
		return s;
	}
	
	private String state(String s)
	{
		s=s.replaceAll("blessed", "ף �� �� ");
		s=s.replaceAll("uncursed", "δ �� �� �� ");
		s=s.replaceAll("cursed", "�� �� �� ");
		return s;
	}
	
	private String hear(String s)
	{
		s=s.replaceAll("You hear some noises in the distance", "�� �� �� Զ �� �� һ Щ �� �� ");
		s=s.replaceAll("You hear some noises", "�� �� �� һ Щ �� �� ");
		s=s.replaceAll("You hear bubbling water", "�� �� �� �� �� �� ˮ �� ");
		s=s.replaceAll("You hear water falling on coins", "�� �� �� ˮ �� �� �� �� �� �� �� �� ");
		s=s.replaceAll("You hear a door open", "�� �� �� �� �� �� �� �� �� ");
		return s;
	}
	
	private String door(String s)
	{
		s=s.replaceAll("The door opens", "�� �� �� ");
		s=s.replaceAll("The door closes", "�� �� �� ");
		s=s.replaceAll("The door resists", "�� Щ �� ");
		s=s.replaceAll("This door is already open", "�� �� �� �� �� ");
		s=s.replaceAll("This door is already closed", "�� �� �� �� �� ");
		s=s.replaceAll("As you kick the door, it crashes open", "�� �� �� �� �� �� ");
		return s;
	}
	
	private String boulder(String s)
	{
		s=s.replaceAll("You try to move the boulder, but in vain", 
				"�� �� ͼ �� �� �� ʯ �� �� �� û �� ");
		s=s.replaceAll("With great effort you move the boulder", 
				"�� �� ȫ �� �� �� �� ʯ ");
		return s;
	}
	
	private String pet(String s)
	{
		s=s.replaceAll("displaced ", "�� �� �� λ �� �� ");
		s=s.replaceAll("picks up", "�� �� �� ");
		s=s.replaceAll("drops", "�� �� �� ");
		s=s.replaceAll("You stop", "�� ͣ �� �� �� ");
		s=s.replaceAll("Your ", "�� �� ");
		s=s.replaceAll("your ", "�� �� ");
		s=s.replaceAll("is in the way", "�� �� · �� ");
		return s;
	}
	
	private String attack(String s)
	{
		s=s.replaceAll("You ", "�� ");
		s=s.replaceAll("just ", "");
		s=s.replaceAll(" misses", "û �� �� ");
		s=s.replaceAll("miss ", "û �� �� ");
		s=s.replaceAll(" bites", "ҧ �� һ �� ");
		s=s.replaceAll("hits", "�� �� һ �� ");
		s=s.replaceAll("are hit ", "�� �� �� ");
		s=s.replaceAll("hit ", "�� �� һ �� ");
		s=s.replaceAll("killed", "�� ɱ �� �� ");
		s=s.replaceAll("kill ", "ɱ �� �� ");
		s=s.replaceAll("destroy ", "�� �� �� ");
		s=s.replaceAll("die", "�� �� ");
		return s;
	}
	
	private String monster(String s)
	{
		s=s.replaceAll("The ", "");
		s=s.replaceAll("the ", "");
		s=s.replaceAll("grid bug", "�� �� �� ");
		s=s.replaceAll("newt", "�� � ");
		s=s.replaceAll("fox", "�� �� ");
		s=s.replaceAll("kobold zombie", "С �� �� ʬ ");
		s=s.replaceAll("goblin", "С �� �� ");
		s=s.replaceAll("giant rat", "�� �� ");
		s=s.replaceAll("gecko", "�� �� ");
		s=s.replaceAll("lichen", "�� �� ");
		s=s.replaceAll("iguana", "�� �� ");
		return s;
	}
	
	private String info(String s)
	{
		int t=-1;
		s=s.replaceAll("Really save", "�� �� �� �� ");
		s=s.replaceAll("Really quit", "ȷ �� �� �� ");
		s=s.replaceAll("In what direction", "�� �� �� �� ");
		s=s.replaceAll("More", "�� �� ");
		s=s.replaceAll(" gold pieces", "�� �� ");
		s=s.replaceAll("see here ", "�� �� �� �� ");
		s=s.replaceAll("Things that are here", "�� �� �� ");
		s=s.replaceAll("Welcome to experience level ", "�� ϲ �� �� �� ");
		s=s.replaceAll("There's some graffiti on the floor here", "�� �� �� д �� �� ");
		
		t=s.indexOf("hear");
		if(t>=0)
		{
			s=hear(s);
		}
		t=s.indexOf("door");
		if(t>=0)
		{
			s=door(s);
		}
		t=s.indexOf("boulder");
		if(t>=0)
		{
			s=boulder(s);
		}
		s=pet(s);
		s=monster(s);
		s=attack(s);
		return s;
	}
	
	private String chalist(String s)
	{
		s=s.replaceAll("Hum", "�� �� ");
		s=s.replaceAll("Dwa", "�� �� ");
		s=s.replaceAll("Elf", "�� �� ");
		s=s.replaceAll("Gno", "٪ �� ");
		s=s.replaceAll("Orc", "�� �� ");
		s=s.replaceAll("Fem", "Ů ");
		s=s.replaceAll("Mal", "�� ");
		s=s.replaceAll("Cha", "�� �� ");
		s=s.replaceAll("Law", "�� �� ");
		s=s.replaceAll("Neu", "�� �� ");
		s=s.replaceAll("Arc", "�� �� ѧ �� ");
		s=s.replaceAll("Bar", "Ұ �� �� ");
		s=s.replaceAll("Cav", "Ѩ �� �� ");
		s=s.replaceAll("Hea", "ҽ �� ");
		s=s.replaceAll("Kni", "�� ʿ ");
		s=s.replaceAll("Mon", "ɮ �� ");
		s=s.replaceAll("Pri", "�� ʦ ");
		s=s.replaceAll("Rog", "�� �� ");
		s=s.replaceAll("Ran", "�� �� ");
		s=s.replaceAll("Sam", "�� ʿ ");
		s=s.replaceAll("Tou", "�� �� �� ");
		s=s.replaceAll("Val", "Ů �� �� ");
		s=s.replaceAll("Wiz", "�� ʦ ");
		return s;
	}
	
	private String character(String s)
	{
		s=s.replaceAll("human", "�� ��");
		s=s.replaceAll("dwarf", "�� ��");
		s=s.replaceAll("dwarven", "�� ��");
		s=s.replaceAll("elf", "�� ��");
		s=s.replaceAll("elven", "�� ��");
		s=s.replaceAll("gnome", "٪ ��");
		s=s.replaceAll("gnomish", "٪ ��");
		s=s.replaceAll("orcish", "�� ��");
		s=s.replaceAll("orc", "�� ��");
		s=s.replaceAll("female", "Ů �� ");
		s=s.replaceAll("male", "�� ��");
		s=s.replaceAll("lawful", "�� ��");
		s=s.replaceAll("neutral", "�� ��");
		s=s.replaceAll("chaotic", "�� ��");
		s=s.replaceAll("Archeologist", "�� �� ѧ �� ");
		s=s.replaceAll("Barbarian", "Ұ �� �� ");
		s=s.replaceAll("Caveman/Cavewoman", "Ѩ �� �� ");
		s=s.replaceAll("Caveman", "Ѩ �� �� ");
		s=s.replaceAll("Cavewoman", "Ѩ �� �� ");
		s=s.replaceAll("Healer", "ҽ �� ");
		s=s.replaceAll("Knight", "�� ʿ ");
		s=s.replaceAll("Monk", "ɮ �� ");
		s=s.replaceAll("Priest/Priestess", "�� ʦ ");
		s=s.replaceAll("Priestess", "�� ʦ ");
		s=s.replaceAll("Priest", "�� ʦ ");
		s=s.replaceAll("Rogue", "�� �� ");
		s=s.replaceAll("Ranger", "�� �� ");
		s=s.replaceAll("Samurai", "�� ʿ ");
		s=s.replaceAll("Tourist", "�� �� �� ");
		s=s.replaceAll("Valkyrie", "Ů �� �� ");
		s=s.replaceAll("Wizard", "�� ʦ ");
		return s;
	}
	
	private String newly(String s)
	{
		s=s.replaceAll("Digger", "�� �� �� ");
		s=s.replaceAll("Plunderer", "�� �� �� ");
		s=s.replaceAll("Plunderess", "�� �� �� ");
		s=s.replaceAll("Troglodyte", "Ѩ �� �� ");
		s=s.replaceAll("Rhizotomist", "�� ҩ ʦ ");
		s=s.replaceAll("Gallant", "�� �� ");
		s=s.replaceAll("Candidate", "С �� �� ");
		s=s.replaceAll("Aspirant", "Ұ �� �� ");
		s=s.replaceAll("Footpad", "�� · �� ");
		s=s.replaceAll("Tenderfoot", "�� ѧ �� ");
		s=s.replaceAll("Hatamoto", "�� �� ");
		s=s.replaceAll("Rambler", "�� �� �� ");
		s=s.replaceAll("Stripling", "�� �� �� ");
		s=s.replaceAll("Evoker", "ħ �� �� ʦ ");
		return s;
	}
	
	private String god(String s)
	{
		s=s.replaceAll("Quetzalcoatl:", "�� �� �� ֮ �� �� ");
		s=s.replaceAll("Quetzalcoatl!", "�� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Quetzalcoatl", "�� �� �� ");
		s=s.replaceAll("Camaxtli:", "�� �� ɪ �� �� ֮ �� �� ");
		s=s.replaceAll("Camaxtli!", "�� �� ɪ �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Camaxtli", "�� �� ɪ �� �� ");
		s=s.replaceAll("Crom:", "�� �� ķ ֮ �� �� ");
		s=s.replaceAll("Crom!", "�� �� ķ �� �� ͬ �� �� ");
		s=s.replaceAll("Crom", "�� �� ķ ");
		s=s.replaceAll("Set:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Set!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Set", "�� �� ");
		s=s.replaceAll("Anu:", "�� Ŭ ֮ �� �� ");
		s=s.replaceAll("Anu!", "�� Ŭ �� �� ͬ �� �� ");
		s=s.replaceAll("Anu", "�� Ŭ ");
		s=s.replaceAll("Ishtar:", "�� ʦ �� ֮ �� �� ");
		s=s.replaceAll("Ishtar!", "�� ʦ �� �� �� ͬ �� �� ");
		s=s.replaceAll("Ishtar", "�� ʦ �� ");
		s=s.replaceAll("Hermes:", "�� �� ī ˹ ֮ �� �� ");
		s=s.replaceAll("Hermes!", "�� �� ī ˹ �� �� ͬ �� �� ");
		s=s.replaceAll("Hermes", "�� �� ī ˹ ");
		s=s.replaceAll("Lugh:", "³ �� ֮ �� �� ");
		s=s.replaceAll("Lugh!", "³ �� �� �� ͬ �� �� ");
		s=s.replaceAll("Lugh", "³ �� ");
		s=s.replaceAll("Shan Lai Ching:", "ɽ �� �� ֮ �� �� ");
		s=s.replaceAll("Shan Lai Ching!", "ɽ �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Shan Lai Ching", "ɽ �� �� ");
		s=s.replaceAll("Chih Sung-tzu:", "�� �� �� ֮ �� �� ");
		s=s.replaceAll("Chih Sung-tzu!", "�� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Chih Sung-tzu", "�� �� �� ");
		s=s.replaceAll("Huan Ti:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Huan Ti!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Huan Ti", "�� �� ");
		s=s.replaceAll("Ptah:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Ptah!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Ptah", "�� �� ");
		s=s.replaceAll("Mercury:", "ī �� �� ֮ �� �� ");
		s=s.replaceAll("Mercury!", "ī �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Mercury", "ī �� �� ");
		s=s.replaceAll("Venus:", "ά �� ˹ ֮ �� �� ");
		s=s.replaceAll("Venus!", "ά �� ˹ �� �� ͬ �� �� ");
		s=s.replaceAll("Venus", "ά �� ˹ ");
		s=s.replaceAll("Manannan Mac Lir:", "�� �� �� �� �� �� �� �� �� ֮ �� �� ");
		s=s.replaceAll("Manannan Mac Lir!", "�� �� �� �� �� �� �� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Manannan Mac Lir", "�� �� �� �� �� �� �� �� �� ");
		s=s.replaceAll("Susanowo:", "�� �� ֮ �� ֮ �� �� ");
		s=s.replaceAll("Susanowo!", "�� �� ֮ �� �� �� ͬ �� �� ");
		s=s.replaceAll("Susanowo", "�� �� ֮ �� ");
		s=s.replaceAll("Offler:", "�� �� �� ֮ �� �� ");
		s=s.replaceAll("Offler!", "�� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Offler", "�� �� �� ");
		s=s.replaceAll("Huhetotl:", "�� �� �� �� �� ֮ �� �� ");
		s=s.replaceAll("Huhetotl!", "�� �� �� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Huhetotl", "�� �� �� �� �� ");
		s=s.replaceAll("Mars:", "�� �� ˹ ֮ �� �� ");
		s=s.replaceAll("Mars!", "�� �� ˹ �� �� ͬ �� �� ");
		s=s.replaceAll("Mars", "�� �� ˹ ");
		s=s.replaceAll("Athena:", "�� �� �� ֮ �� �� ");
		s=s.replaceAll("Athena!", "�� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Athena", "�� �� �� ");
		s=s.replaceAll("Amaterasu Omikami:", "�� �� Ů �� ֮ �� �� ");
		s=s.replaceAll("Amaterasu Omikami!", "�� �� Ů �� �� �� ͬ �� �� ");
		s=s.replaceAll("Amaterasu Omikami", "�� �� Ů �� ");
		s=s.replaceAll("Blind Io:", "ä ľ �� ֮ �� �� ");
		s=s.replaceAll("Blind Io!", "ä ľ �� �� �� ͬ �� �� ");
		s=s.replaceAll("Blind Io", "ä ľ �� ");
		s=s.replaceAll("The Lady:", "�� ɽ �� �� ֮ �� �� ");
		s=s.replaceAll("The Lady!", "�� ɽ �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("The Lady", "�� ɽ �� �� ");
		s=s.replaceAll("Brigit:", "�� �� �� �� ֮ �� �� ");
		s=s.replaceAll("Brigit!", "�� �� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Brigit", "�� �� �� �� ");
		s=s.replaceAll("Kos:", "�� ˹ ֮ �� �� ");
		s=s.replaceAll("Kos!", "�� ˹ �� �� ͬ �� �� ");
		s=s.replaceAll("Kos", "�� ˹ ");
		s=s.replaceAll("Mitra:", "�� �� �� ֮ �� �� ");
		s=s.replaceAll("Mitra!", "�� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Mitra", "�� �� �� ");
		s=s.replaceAll("Raijin:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Raijin!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Raijin", "�� �� ");
		s=s.replaceAll("Mog:", "Ī �� ֮ �� �� ");
		s=s.replaceAll("Mog!", "Ī �� �� �� ͬ �� �� ");
		s=s.replaceAll("Mog", "Ī �� ");
		s=s.replaceAll("Thoth:", "͸ �� ֮ �� �� ");
		s=s.replaceAll("Thoth!", "͸ �� �� �� ͬ �� �� ");
		s=s.replaceAll("Thoth", "͸ �� ");
		s=s.replaceAll("Anshar:", "�� ɳ �� ֮ �� �� ");
		s=s.replaceAll("Anshar!", "�� ɳ �� �� �� ͬ �� �� ");
		s=s.replaceAll("Anshar", "�� ɳ �� ");
		s=s.replaceAll("Poseidon:", "�� �� �� ֮ �� �� ");
		s=s.replaceAll("Poseidon!", "�� �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Poseidon", "�� �� �� ");
		s=s.replaceAll("Loki:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Loki!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Loki", "�� �� ");
		s=s.replaceAll("Tyr:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Tyr!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Tyr", "�� �� ");
		s=s.replaceAll("Odin:", "ŷ �� �� ֮ �� �� ");
		s=s.replaceAll("Odin!", "ŷ �� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Odin", "ŷ �� �� ");
		s=s.replaceAll("Anhur:", "�� �� ֮ �� �� ");
		s=s.replaceAll("Anhur!", "�� �� �� �� ͬ �� �� ");
		s=s.replaceAll("Anhur", "�� �� ");
		return s;
	}
}
