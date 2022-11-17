#!/bin/bash

# 拷贝文件过来
function mkdir_and_cp(){
    local file=$1
    mkdir -p ${page_path}/${file} ${page_path}/${file}/pic
    cp -r ${note_path}/${file}/{pic,*.mkd,*.markdown,*.md,*.html,*.htm}    ${page_path}/${file}
}

function clear_files(){
    mv ${page_path}/.git    ${page_path}/../git
    # 清空原目录
    echo "清空目录:${page_path}"
    rm -rf "${page_path}"

    # 重新创建目录
    echo "创建目录:${page_path}"
    mkdir -p "${page_path}" 
    mv ${page_path}/../git  ${page_path}/.git
}


function fetch_files(){
    # fetch files
    dirs=`dir ${note_path}`
    for i in $(echo ${dirs})
    do 
        echo -en ${note_path}/${i}
        if [ -d ${note_path}/${i} ]
        then
            echo -en "是目录\n"
            mkdir_and_cp ${i}
        else
            echo -en "----------------------------------------------------\n"
            cp ${note_path}/${i} ${page_path}
        fi
    done
}

function loop_add_yaml(){
    # 添加YAML头
    local i=$1
    if [[ ! -s $i ]]
    then
        echo -en "空文件$i\n"
        echo -en "---\nlayout: post\n---\n空文件\n">>$i
        return
    fi
    if [[ `head -c 3 "$i"` == "---" ]]
    then
        #echo -en `head -c 3 "$i"`
        echo -en "$i已经有YAML头\n"
    else
        echo -en "正在为$i添加YAML头\n"
        # 取自动标题
        file_title=${i##*/}
        file_title=${file_title%.*}
        # 添加YAML头
        #sed -i "1i---\nlayout: post\ntitle: ${file_title}\n---\n" $i
        sed -i "1i---\nlayout: post\n---\n" $i
    fi
}

function add_yaml(){
    # 遍历每一个文件，为markdown添加yaml头
    echo -en "循环开始！\n"
    mds=${page_path}/**/*.md
    mds="${page_path}/*.md ${mds}"
    for i in $(echo ${mds})
    do
        #echo $i
        # 添加YAML头
        loop_add_yaml $i
    done
    echo -e "循环结束！"
}

function main(){
    #测试指定目录
    page_path="/D/Studio/repos/viccch.github.ioo/ff"
    note_path="/D/file/classes"
    # 清理目录
    # 取文件
    # 添加yaml头
    clear_files
    fetch_files
    note_path="/D/file/classes/old"
    fetch_files
    add_yaml
}


# SCRIPT START
main

