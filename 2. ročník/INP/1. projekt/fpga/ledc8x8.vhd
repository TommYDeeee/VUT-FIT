-- Autor reseni: SEM DOPLNTE VASE, JMENO, PRIJMENI A LOGIN

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
	SMCLK : in std_logic;
	RESET : in std_logic;
	ROW : out std_logic_vector (0 to 7);
	LED : out std_logic_vector (0 to 7)
);
end ledc8x8;

architecture main of ledc8x8 is

	signal rows : std_logic_vector(0 to 7) := "10000000";
	signal leds : std_logic_vector(0 to 7) := (others => '1');
	signal rise : std_logic := '0';
	signal switch_state : std_logic_vector(0 to 1) := "00";
	signal counter : std_logic_vector(11 downto 0) := (others => '0');
	signal counter2 : std_logic_vector(23 downto 0) := (others => '0');
	signal state : std_logic := '0';
	
begin

	process(SMCLK, RESET)
	begin
		if(RESET = '1') then
			counter <= (others => '0');
		elsif (rising_edge(SMCLK)) then
			if switch_state = "00" or switch_state = "01" then
				if switch_state /= "10" then
					counter2 <= counter2 + 1;
				end if;
			end if;
			
			if (counter2 = 	 "1110000100000000000000") then
				switch_state <= "01";
			elsif (counter2 = "11100001000000000000000") then
				switch_state <= "10";
				counter2 <= (others => '0');
			end if;
			
			if counter = "111000010000" then
				counter <= (others => '0');
				rise <= '1';
			else
				counter <= counter + 1;
				rise <= '0';
			end if;		

			case switch_state is
				when "00" => state <= '0';
				when "01" => state <= '1';
				when "10" => state <= '0';
				when others => null;
			end case;
		end if;
	end process;
		
process(RESET, SMCLK, rise)
    begin
        if RESET = '1' then
            rows <= "10000000";
        elsif rising_edge(SMCLK) and rise = '1' then
				case rows is
                when "10000000" => rows <= "01000000";
                when "01000000" => rows <= "00100000";
                when "00100000" => rows <= "00010000";
                when "00010000" => rows <= "00001000";
                when "00001000" => rows <= "00000100";
                when "00000100" => rows <= "00000010";
                when "00000010" => rows <= "00000001";
                when "00000001" => rows <= "10000000";
                when others => null;
            end case;
        end if;
    end process;

	
	process(state, rows)
	begin
		if (state = '0') then
			case (rows) is				
				when "10000000" => leds <= "00000111";
				when "01000000" => leds <= "11011111";
				when "00100000" => leds <= "11010011";
				when "00010000" => leds <= "11010101";
				when "00001000" => leds <= "11010101";
				when "00000100" => leds <= "11110101";
				when "00000010" => leds <= "11110011";
				when "00000001" => leds <= "11111111";
				when others => leds <= "11111111";
			end case;
		else 
			leds <= "11111111";
		end if;
	end process;
	ROW <= rows;
	LED <= leds;
end main;



-- ISID: 75579
