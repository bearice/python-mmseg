#!/usr/bin/python
# -*- coding: utf8 -*-
import mmseg
mmseg.init()

str = "随着人们对食品安全的关注越来越多，任何现代加工技术都会带来疑虑。许多人相信，传统的、以前的加工技术总是安全和健康的。比如食用油，“自家榨的油”甚至成为了高档的礼品，就象“有机产品”一样受到人们的追逐。"

tokens = mmseg.tokenlize(str)

for (pos,len,tok) in tokens:
	print pos,len,tok.decode('utf8')

