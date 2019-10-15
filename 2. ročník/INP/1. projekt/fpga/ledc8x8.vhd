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
	signal switch_state : std_logic := '0';
	signal counter : std_logic_vector(0 to 7) := (others => '0');
	signal counter2 : std_logic_vector(0 to 23) := (others => '0');
	signal dont_switch : std_logic_vector(0 to 1) := "00";
	signal state: std_logic_vector(0 to 1) := "00";
	
begin

	process(SMCLK, RESET)
	begin
		if(RESET = '1') then
			counter <= (others => '0');
		elsif (rising_edge(SMCLK)) then
			counter <= counter + 1;
			counter2 <= counter2 + 1;
		end if;
		
		if (counter2 = "1110000100000000000000") then
			switch_state <= '1';
		elsif (counter2 = "11100001000000000000000") then
			switch_state <= '1';
		elsif (counter2 = "111000010000000000000000") then
			switch_state <= '1';
			dont_switch <= "01";
		else 
			switch_state <= '0';
		end if;
	end process;
	rise <= '1' when counter = "11111111" else '0';
	
	
process(RESET, SMCLK, counter)
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
		  
		  if(switch_state = '1') and (dont_switch /= "01") then

					case state is
						when "00" => state <= "11";
						when "11" => state <= "00";
						when others => state <= "00";
					end case;
			end if;
    end process;

	
	process(state, rows)
	begin
		if (state = "00") then
			case (rows) is				
				when "10000000" =>
					leds <= "00000111";
				when "01000000" =>
					leds <= "11011111";
				when "00100000" =>
					leds <= "11010011";
				when "00010000" =>
					leds <= "11010101";
				when "00001000" =>
					leds <= "11010101";
				when "00000100" =>
					leds <= "11110101";
				when "00000010" =>
					leds <= "11110011";
				when "00000001" =>
					leds <= "11111111";
				when others =>
					leds <= "11111111";
			end case;
		
		elsif (state = "10") then
			case (rows)  is			
				when "10000000" =>
					leds <= "11111111";
				when "01000000" =>
					leds <= "11111111";
				when "00100000" =>
					leds <= "11111111";
				when "00010000" =>
					leds <= "11111111";
				when "00001000" =>
					leds <= "11111111";
				when "00000100" =>
					leds <= "11111111";
				when "00000010" =>
					leds <= "11111111";
				when "00000001" =>
					leds <= "11111111";
				when others =>
					leds <= "11111111";
			end case;
			
		else 
			leds <= "11111111";
		end if;
	end process;
	ROW <= rows;
	LED <= leds;
end main;



-- ISID: 75579
