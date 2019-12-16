-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2019 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu
 signal pc			:	std_logic_vector(12 downto 0);
 signal pc_inc		:	std_logic;
 signal pc_dec		:	std_logic;
 
 signal cnt			:	std_logic_vector(7 downto 0);
 signal cnt_inc	:	std_logic;
 signal cnt_dec	:	std_logic;
 
 signal ptr			:	std_logic_vector(12 downto 0);
 signal ptr_inc	:	std_logic;
 signal ptr_dec	:	std_logic;
 
 signal ptr_tmp   :  std_logic_vector(12 downto 0);
 signal pc_tmp    :  std_logic_vector(12 downto 0);
 signal mux1_sel  :  std_logic;
 signal mux2_sel  :  std_logic;
 signal mux3_sel  :  std_logic_vector(1 downto 0);
 signal ptr_out_tmp: std_logic_vector(12 downto 0);
 
 
 
 type state is (
	idle,
	fetch,
	decode,
	pointer_inc, pointer_dec,
	value_inc, value_inc_write, value_dec, value_dec_write,
	putchar, putchar_2, getchar,
	other_state,
	save_to_tmp, save_from_tmp, save_to_tmp_2, save_from_tmp_2,
	while_start, while_start_2, while_start_3, while_start_4, while_start_5,
	while_end, while_end_2, while_end_3, while_end_4, while_end_5, while_end_6,
	comment,
	state_end
 );

 signal next_state		:	state;
 signal current_state	:	state;

begin

 -- zde dopiste vlastni VHDL kod


 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --   - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --   - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.
 
	pc_process: process(CLK,RESET,pc_tmp,pc,pc_inc,pc_dec)
 
	begin
		if (RESET = '1') then
			pc <= "0000000000000";
		elsif (CLK'event) and (CLK = '1') then
			if(pc_inc = '0' and pc_dec = '1') then
				pc <= pc - 1;
			elsif( pc_inc = '1' and pc_dec = '0') then
				pc <= pc + 1;
			end if;
		end if;
		pc_tmp <= pc;
	end process;

	cnt_process	:	process(CLK,RESET,cnt,cnt_inc,cnt_dec) 
	
	begin
		if (RESET = '1') then
			cnt <= "00000000";
		elsif (CLK'event) and (CLK = '1') then
			if(cnt_inc = '0' and cnt_dec = '1') then
				cnt <= cnt - 1;
			elsif( cnt_inc = '1' and cnt_dec = '0') then
				cnt <= cnt + 1;
			end if;
		end if;
	end process;
 
 	ptr_process	:	process(CLK,RESET,ptr_tmp,ptr,ptr_inc,ptr_dec) --ukazatel do pamate
	
	begin
		if (RESET = '1') then
			ptr <= "1000000000000";
		elsif (CLK'event) and (CLK = '1') then
			if(ptr_inc = '0' and ptr_dec = '1') then
				ptr <= ptr - 1;
			elsif( ptr_inc = '1' and ptr_dec = '0') then
				if ptr = "1111111111111" then
					ptr <= "1000000000000";
				else
					ptr <= ptr + 1;
				end if;
			elsif ptr_inc = '0' and ptr_dec = '1' then
				if ptr = "1000000000000" then
					ptr <= "1111111111111";
				else 
					ptr <= ptr - 1;
				end if;
			end if;
		end if;
		ptr_tmp <= ptr;
	end process;
	
	fsm_process : process(CLK,RESET)
	
	begin
		if (RESET = '1') then
			current_state <= idle;
		elsif (CLK'event) and (CLK = '1') then
			if (EN = '1') then
				current_state <= next_state;
			end if;
		end if;		
	end process;
	
	mux2: process(ptr_out_tmp, mux2_sel, ptr_tmp)
	
	begin

		case mux2_sel is
			when '1' => ptr_out_tmp <= ptr_tmp;
			when '0' => ptr_out_tmp <= "1000000000000";
			when others =>
		end case;

	end process;
	
	mux1: process(ptr_out_tmp, mux1_sel, pc_tmp)
	
	begin
			
			case mux1_sel is
			when '1' => DATA_ADDR <= pc_tmp;
			when '0' => DATA_ADDR <= ptr_out_tmp;
			when others =>
		end case;

	end process;
	
	mux3: process(IN_DATA, DATA_RDATA, mux3_sel)
	
	begin 
	
			case mux3_sel is
			when "00" => DATA_WDATA <= IN_DATA;
			when "01" => DATA_WDATA <= DATA_RDATA - 1;
			when "10" => DATA_WDATA <= DATA_RDATA + 1;
			when "11" => DATA_WDATA <= DATA_RDATA;
			when others =>
		end case;

	end process;
			
	OUT_DATA	<= DATA_RDATA;
	
	fsm_state: process(CLK, RESET, EN, DATA_RDATA, IN_VLD, OUT_BUSY)
	
	begin
	
		next_state <= idle;
		
		DATA_EN  <= '0';
		OUT_WE   <= '0';
		IN_REQ   <= '0';
		
		pc_inc   <= '0';
		pc_dec   <= '0';
		
		ptr_inc  <= '0';
		ptr_dec  <= '0';
		
		cnt_inc  <= '0';
		cnt_dec  <= '0';
		
		mux3_sel <= "00";
		mux1_sel <= '0';
		mux2_sel <= '0';
	
		case current_state is
		
			when idle =>
				next_state <= fetch;
				
			when fetch =>
				
				DATA_EN    <= '1';
				DATA_RDWR  <= '0';
				mux1_sel   <= '1';
				next_state <= decode; 
				
			
			when decode =>
				case (DATA_RDATA) is
						when X"3E" =>
							next_state <= pointer_inc;
						when X"3C" =>
							next_state <= pointer_dec;
						when X"2B" =>
							next_state <= value_inc;
						when X"2D" =>
							next_state <= value_dec;
						when X"2E" =>
							next_state <= putchar;
						when X"2C" =>
							next_state <= getchar;
						when X"00" =>
							next_state <= state_end;
						when X"24" =>
							next_state <= save_to_tmp;
						when X"21" => 
							next_state <= save_from_tmp;
						when X"5B" => 
							next_state <= while_start;
						when X"5D" =>
							next_state <= while_end;
						when others =>
							next_state <= comment;
					end case;
				
	-- ---------------------------------------				
				when pointer_inc =>
					pc_inc <= '1';
					ptr_inc <= '1';
					next_state <= fetch;
	-- ---------------------------------------				
				when pointer_dec =>
					pc_inc <= '1';
					ptr_dec <= '1';
					next_state <= fetch;	
	-- ---------------------------------------				
				when value_inc =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= value_inc_write;
	-- ---------------------------------------				
				when value_inc_write =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					mux3_sel <= "10";
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					pc_inc <= '1';
					next_state <= fetch;	
	-- ---------------------------------------			
				when value_dec =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= value_dec_write;
	-- ---------------------------------------				
				when value_dec_write =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					mux3_sel <= "01";
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					pc_inc <= '1';
					next_state <= fetch;	
	-- ---------------------------------------------				
				when putchar =>	
					if(OUT_BUSY = '1') then
						next_state <= putchar;
					else
						next_state	<= putchar_2;
						mux1_sel <= '0';
						mux2_sel <= '1';
						DATA_EN     <= '1'; 					
						DATA_RDWR	<= '0';		
					end if;
	-- ---------------------------------------
				when putchar_2 =>
					next_state	<= fetch;
					OUT_WE		<= '1';
					pc_inc		<= '1';	
	-- ---------------------------------------		
				when getchar =>
					IN_REQ <= '1';
					if(IN_VLD = '1') then
						mux1_sel <= '0';
						mux2_sel <= '1';
						mux3_sel <= "00";
						DATA_EN <= '1';
						DATA_RDWR <= '1';
						pc_inc <= '1';
						next_state <= fetch;
					else
					  next_state <= getchar;
					 end if;
	-- ---------------------------------------	
				when save_to_tmp =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= save_to_tmp_2;
	-- ---------------------------------------				
				when save_to_tmp_2 =>
					mux1_sel <= '0';
					mux2_sel <= '0';
					mux3_sel <= "11";
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					pc_inc <= '1';
					next_state <= fetch;
	-- ---------------------------------------	
				when save_from_tmp =>
					mux1_sel <= '0';
					mux2_sel <= '0';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= save_from_tmp_2;			
	-- ---------------------------------------			
				when save_from_tmp_2 =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					mux3_sel <= "11";
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					pc_inc <= '1';
					next_state <= fetch;			
	-- ---------------------------------------	
				when while_start =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					pc_inc <= '1';
					next_state <= while_start_2;
	-- ---------------------------------------			
				when while_start_2 =>
					if (DATA_RDATA = "00000000") then
						cnt_inc <= '1';
						next_state <= while_start_3;
					else
						next_state <= fetch;
					end if;
	-- ---------------------------------------					
				when while_start_3 =>
					mux1_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= while_start_4;
	-- ---------------------------------------				
				when while_start_4 =>
					pc_inc <= '1';
					if (DATA_RDATA = X"5B") then
						cnt_inc <= '1';
					elsif (DATA_RDATA = X"5D") then
						cnt_dec <= '1';
					end if;
					next_state <= while_start_5;
	-- ---------------------------------------				
				when while_start_5 =>
					if ( cnt = "00000000") then
						next_state <= fetch;
					else
						next_state <= while_start_3;
					end if;
	-- ---------------------------------------	
				when while_end =>
					mux1_sel <= '0';
					mux2_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= while_end_2;
	-- ---------------------------------------	
				when while_end_2 =>
					if (DATA_RDATA = "00000000") then
						pc_inc <= '1';
						next_state <= fetch;
					else 
						cnt_inc <= '1';
						pc_dec <= '1';
						next_state <= while_end_3;
					end if;
	-- ---------------------------------------	
				when while_end_3 =>
					mux1_sel <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					next_state <= while_end_4;
	-- ---------------------------------------	
				when while_end_4 =>
					if (DATA_RDATA = X"5D") then
						cnt_inc <= '1';
					elsif (DATA_RDATA = X"5B") then
						cnt_dec <= '1';
					end if;
					next_state <= while_end_5;
	-- ---------------------------------------	
				when while_end_5 =>
					if (cnt = "00000000") then
						pc_inc <= '1';
					else 
						pc_dec <= '1';
					end if;
					next_state <= while_end_6;
	-- ---------------------------------------	
				when while_end_6 =>
					if (cnt = "00000000") then
						next_state <= fetch;
					else 
						next_state <= while_end_3;
					end if;
	-- ---------------------------------------	
				when comment =>
					next_state <= fetch;
					pc_inc <= '1';	
	-- ---------------------------------------		
				when state_end =>
					next_state <= state_end;
	-- ---------------------------------------	
				when others =>
			end case;
	end process;
	 
end behavioral;
	 
