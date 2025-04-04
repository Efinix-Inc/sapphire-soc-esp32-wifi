--------------------------------------------------------------------------------
-- Copyright (C) 2013-2024 Efinix Inc. All rights reserved.              
--
-- This   document  contains  proprietary information  which   is        
-- protected by  copyright. All rights  are reserved.  This notice       
-- refers to original work by Efinix, Inc. which may be derivitive       
-- of other work distributed under license of the authors.  In the       
-- case of derivative work, nothing in this notice overrides the         
-- original author's license agreement.  Where applicable, the           
-- original license agreement is included in it's original               
-- unmodified form immediately below this header.                        
--                                                                       
-- WARRANTY DISCLAIMER.                                                  
--     THE  DESIGN, CODE, OR INFORMATION ARE PROVIDED “AS IS” AND        
--     EFINIX MAKES NO WARRANTIES, EXPRESS OR IMPLIED WITH               
--     RESPECT THERETO, AND EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTIES,  
--     INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF          
--     MERCHANTABILITY, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR    
--     PURPOSE.  SOME STATES DO NOT ALLOW EXCLUSIONS OF AN IMPLIED       
--     WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO LICENSEE.           
--                                                                       
-- LIMITATION OF LIABILITY.                                              
--     NOTWITHSTANDING ANYTHING TO THE CONTRARY, EXCEPT FOR BODILY       
--     INJURY, EFINIX SHALL NOT BE LIABLE WITH RESPECT TO ANY SUBJECT    
--     MATTER OF THIS AGREEMENT UNDER TORT, CONTRACT, STRICT LIABILITY   
--     OR ANY OTHER LEGAL OR EQUITABLE THEORY (I) FOR ANY INDIRECT,      
--     SPECIAL, INCIDENTAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES OF ANY    
--     CHARACTER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF      
--     GOODWILL, DATA OR PROFIT, WORK STOPPAGE, OR COMPUTER FAILURE OR   
--     MALFUNCTION, OR IN ANY EVENT (II) FOR ANY AMOUNT IN EXCESS, IN    
--     THE AGGREGATE, OF THE FEE PAID BY LICENSEE TO EFINIX HEREUNDER    
--     (OR, IF THE FEE HAS BEEN WAIVED, $100), EVEN IF EFINIX SHALL HAVE 
--     BEEN INFORMED OF THE POSSIBILITY OF SUCH DAMAGES.  SOME STATES DO 
--     NOT ALLOW THE EXCLUSION OR LIMITATION OF INCIDENTAL OR            
--     CONSEQUENTIAL DAMAGES, SO THIS LIMITATION AND EXCLUSION MAY NOT   
--     APPLY TO LICENSEE.                                                
--
--------------------------------------------------------------------------------
------------- Begin Cut here for COMPONENT Declaration ------
component gAXIS_1to3_switch is
port (
    rst_n : in std_logic;
    clk : in std_logic;
    s_axi_awvalid : in std_logic_vector(0 to 0);
    s_axi_awaddr : in std_logic_vector(31 downto 0);
    s_axi_awlock : in std_logic_vector(1 downto 0);
    s_axi_awready : out std_logic_vector(0 to 0);
    s_axi_arvalid : in std_logic_vector(0 to 0);
    s_axi_araddr : in std_logic_vector(31 downto 0);
    s_axi_arlock : in std_logic_vector(1 downto 0);
    s_axi_arready : out std_logic_vector(0 to 0);
    s_axi_wvalid : in std_logic_vector(0 to 0);
    s_axi_wlast : in std_logic_vector(0 to 0);
    s_axi_wid : in std_logic_vector(7 downto 0);
    s_axi_bready : in std_logic_vector(0 to 0);
    s_axi_bresp : out std_logic_vector(1 downto 0);
    s_axi_rready : in std_logic_vector(0 to 0);
    s_axi_bid : out std_logic_vector(7 downto 0);
    s_axi_rid : out std_logic_vector(7 downto 0);
    s_axi_wdata : in std_logic_vector(31 downto 0);
    s_axi_rdata : out std_logic_vector(31 downto 0);
    s_axi_rresp : out std_logic_vector(1 downto 0);
    s_axi_bvalid : out std_logic_vector(0 to 0);
    s_axi_rvalid : out std_logic_vector(0 to 0);
    s_axi_rlast : out std_logic_vector(0 to 0);
    s_axi_wstrb : in std_logic_vector(3 downto 0);
    m_axi_awvalid : out std_logic_vector(2 downto 0);
    m_axi_awaddr : out std_logic_vector(95 downto 0);
    m_axi_awlock : out std_logic_vector(5 downto 0);
    m_axi_awready : in std_logic_vector(2 downto 0);
    m_axi_arvalid : out std_logic_vector(2 downto 0);
    m_axi_araddr : out std_logic_vector(95 downto 0);
    m_axi_arlock : out std_logic_vector(5 downto 0);
    m_axi_arready : in std_logic_vector(2 downto 0);
    m_axi_wvalid : out std_logic_vector(2 downto 0);
    m_axi_wlast : out std_logic_vector(2 downto 0);
    m_axi_bready : out std_logic_vector(2 downto 0);
    m_axi_bresp : in std_logic_vector(5 downto 0);
    m_axi_rready : out std_logic_vector(2 downto 0);
    m_axi_bid : in std_logic_vector(23 downto 0);
    m_axi_rid : in std_logic_vector(23 downto 0);
    m_axi_wdata : out std_logic_vector(95 downto 0);
    m_axi_rdata : in std_logic_vector(95 downto 0);
    m_axi_rresp : in std_logic_vector(5 downto 0);
    m_axi_bvalid : in std_logic_vector(2 downto 0);
    m_axi_rvalid : in std_logic_vector(2 downto 0);
    m_axi_rlast : in std_logic_vector(2 downto 0);
    m_axi_wstrb : out std_logic_vector(11 downto 0);
    m_axi_wready : in std_logic_vector(2 downto 0);
    s_axi_wready : out std_logic_vector(0 to 0);
    s_axi_awprot : in std_logic_vector(3 downto 0);
    s_axi_awcache : in std_logic_vector(3 downto 0);
    s_axi_awqos : in std_logic_vector(3 downto 0);
    s_axi_awuser : in std_logic_vector(2 downto 0);
    s_axi_arqos : in std_logic_vector(3 downto 0);
    s_axi_arcache : in std_logic_vector(3 downto 0);
    m_axi_awprot : out std_logic_vector(11 downto 0);
    s_axi_arid : in std_logic_vector(7 downto 0);
    s_axi_arsize : in std_logic_vector(2 downto 0);
    s_axi_arlen : in std_logic_vector(7 downto 0);
    s_axi_arburst : in std_logic_vector(1 downto 0);
    s_axi_arprot : in std_logic_vector(3 downto 0);
    s_axi_awid : in std_logic_vector(7 downto 0);
    s_axi_awburst : in std_logic_vector(1 downto 0);
    s_axi_awlen : in std_logic_vector(7 downto 0);
    s_axi_awsize : in std_logic_vector(2 downto 0);
    m_axi_awid : out std_logic_vector(23 downto 0);
    m_axi_awburst : out std_logic_vector(5 downto 0);
    m_axi_awlen : out std_logic_vector(23 downto 0);
    m_axi_awsize : out std_logic_vector(8 downto 0);
    m_axi_awcache : out std_logic_vector(11 downto 0);
    m_axi_awqos : out std_logic_vector(11 downto 0);
    m_axi_awuser : out std_logic_vector(8 downto 0);
    m_axi_arprot : out std_logic_vector(11 downto 0);
    m_axi_arburst : out std_logic_vector(5 downto 0);
    m_axi_arlen : out std_logic_vector(23 downto 0);
    m_axi_arsize : out std_logic_vector(8 downto 0);
    m_axi_arcache : out std_logic_vector(11 downto 0);
    m_axi_arqos : out std_logic_vector(11 downto 0);
    m_axi_aruser : out std_logic_vector(8 downto 0);
    m_axi_awregion : out std_logic_vector(11 downto 0);
    m_axi_arregion : out std_logic_vector(11 downto 0);
    m_axi_arid : out std_logic_vector(23 downto 0);
    m_axi_wuser : out std_logic_vector(8 downto 0);
    m_axi_ruser : in std_logic_vector(8 downto 0);
    m_axi_buser : in std_logic_vector(8 downto 0);
    s_axi_aruser : in std_logic_vector(2 downto 0);
    s_axi_wuser : in std_logic_vector(2 downto 0);
    s_axi_buser : out std_logic_vector(2 downto 0);
    s_axi_ruser : out std_logic_vector(2 downto 0)
);
end component gAXIS_1to3_switch;

---------------------- End COMPONENT Declaration ------------
------------- Begin Cut here for INSTANTIATION Template -----
u_gAXIS_1to3_switch : gAXIS_1to3_switch
port map (
    rst_n => rst_n,
    clk => clk,
    s_axi_awvalid => s_axi_awvalid,
    s_axi_awaddr => s_axi_awaddr,
    s_axi_awlock => s_axi_awlock,
    s_axi_awready => s_axi_awready,
    s_axi_arvalid => s_axi_arvalid,
    s_axi_araddr => s_axi_araddr,
    s_axi_arlock => s_axi_arlock,
    s_axi_arready => s_axi_arready,
    s_axi_wvalid => s_axi_wvalid,
    s_axi_wlast => s_axi_wlast,
    s_axi_wid => s_axi_wid,
    s_axi_bready => s_axi_bready,
    s_axi_bresp => s_axi_bresp,
    s_axi_rready => s_axi_rready,
    s_axi_bid => s_axi_bid,
    s_axi_rid => s_axi_rid,
    s_axi_wdata => s_axi_wdata,
    s_axi_rdata => s_axi_rdata,
    s_axi_rresp => s_axi_rresp,
    s_axi_bvalid => s_axi_bvalid,
    s_axi_rvalid => s_axi_rvalid,
    s_axi_rlast => s_axi_rlast,
    s_axi_wstrb => s_axi_wstrb,
    m_axi_awvalid => m_axi_awvalid,
    m_axi_awaddr => m_axi_awaddr,
    m_axi_awlock => m_axi_awlock,
    m_axi_awready => m_axi_awready,
    m_axi_arvalid => m_axi_arvalid,
    m_axi_araddr => m_axi_araddr,
    m_axi_arlock => m_axi_arlock,
    m_axi_arready => m_axi_arready,
    m_axi_wvalid => m_axi_wvalid,
    m_axi_wlast => m_axi_wlast,
    m_axi_bready => m_axi_bready,
    m_axi_bresp => m_axi_bresp,
    m_axi_rready => m_axi_rready,
    m_axi_bid => m_axi_bid,
    m_axi_rid => m_axi_rid,
    m_axi_wdata => m_axi_wdata,
    m_axi_rdata => m_axi_rdata,
    m_axi_rresp => m_axi_rresp,
    m_axi_bvalid => m_axi_bvalid,
    m_axi_rvalid => m_axi_rvalid,
    m_axi_rlast => m_axi_rlast,
    m_axi_wstrb => m_axi_wstrb,
    m_axi_wready => m_axi_wready,
    s_axi_wready => s_axi_wready,
    s_axi_awprot => s_axi_awprot,
    s_axi_awcache => s_axi_awcache,
    s_axi_awqos => s_axi_awqos,
    s_axi_awuser => s_axi_awuser,
    s_axi_arqos => s_axi_arqos,
    s_axi_arcache => s_axi_arcache,
    m_axi_awprot => m_axi_awprot,
    s_axi_arid => s_axi_arid,
    s_axi_arsize => s_axi_arsize,
    s_axi_arlen => s_axi_arlen,
    s_axi_arburst => s_axi_arburst,
    s_axi_arprot => s_axi_arprot,
    s_axi_awid => s_axi_awid,
    s_axi_awburst => s_axi_awburst,
    s_axi_awlen => s_axi_awlen,
    s_axi_awsize => s_axi_awsize,
    m_axi_awid => m_axi_awid,
    m_axi_awburst => m_axi_awburst,
    m_axi_awlen => m_axi_awlen,
    m_axi_awsize => m_axi_awsize,
    m_axi_awcache => m_axi_awcache,
    m_axi_awqos => m_axi_awqos,
    m_axi_awuser => m_axi_awuser,
    m_axi_arprot => m_axi_arprot,
    m_axi_arburst => m_axi_arburst,
    m_axi_arlen => m_axi_arlen,
    m_axi_arsize => m_axi_arsize,
    m_axi_arcache => m_axi_arcache,
    m_axi_arqos => m_axi_arqos,
    m_axi_aruser => m_axi_aruser,
    m_axi_awregion => m_axi_awregion,
    m_axi_arregion => m_axi_arregion,
    m_axi_arid => m_axi_arid,
    m_axi_wuser => m_axi_wuser,
    m_axi_ruser => m_axi_ruser,
    m_axi_buser => m_axi_buser,
    s_axi_aruser => s_axi_aruser,
    s_axi_wuser => s_axi_wuser,
    s_axi_buser => s_axi_buser,
    s_axi_ruser => s_axi_ruser
);

------------------------ End INSTANTIATION Template ---------
